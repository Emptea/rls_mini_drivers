#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>
#include <linux/of_address.h>
#include <linux/io.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include "regs.h"

#define DRV_NAME "axi_multiplier"
#define DEV_NAME "axi_multiplier"

#define REG_MAX_OFFSET CSR_MULT7_ADDR
#define REG_SIZE       sizeof(u32)

struct axi_mult_dev {
    struct device *dev;
    void __iomem *base;
    struct mutex lock;
    dev_t devno;
    struct cdev cdev;
    struct class *class;
    struct device *device;
};

static bool reg_is_valid(u32 off)
{
    switch (off) {
    case CSR_IP_VER_ADDR:
    case CSR_KILL_ADDR:
    case CSR_TEST_POINT_ADDR:
    case CSR_CHANNEL_ADDR:
    case CSR_MULT0_ADDR:
    case CSR_MULT1_ADDR:
    case CSR_MULT2_ADDR:
    case CSR_MULT3_ADDR:
    case CSR_MULT4_ADDR:
    case CSR_MULT5_ADDR:
    case CSR_MULT6_ADDR:
    case CSR_MULT7_ADDR:
        return true;
    default:
        return false;
    }
}

static bool reg_is_writable(u32 off)
{
    return off != REG_OFFSET_IP_VER;
}

static loff_t axi_llseek(struct file *filp, loff_t off, int whence)
{
    loff_t newpos;

    switch (whence) {
    case SEEK_SET:
        newpos = off;
        break;
    default:
        return -EINVAL;
    }

    if (newpos < 0 || newpos > REG_MAX_OFFSET)
        return -EINVAL;

    if (newpos % REG_SIZE)
        return -EINVAL;

    filp->f_pos = newpos;
    return newpos;
}

static int axi_open(struct inode *inode, struct file *filp)
{
    struct axi_mult_dev *d = container_of(inode->i_cdev, struct axi_mult_dev, cdev);
    filp->private_data = d;
    return 0;
}

static ssize_t axi_mult_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
    struct axi_mult_dev *d = filp->private_data;
    u32 value;
    u32 reg_off = *off;

    if (len < sizeof(value))
        return -EINVAL;

    if (!reg_is_valid(reg_off))
        return -EINVAL;

    mutex_lock(&d->lock);
    value = readl(d->base + reg_off);
    mutex_unlock(&d->lock);

    if (copy_to_user(buf, &value, sizeof(value)))
        return -EFAULT;

    *off += sizeof(value);
    dev_info(d->dev, "read reg 0x%02x = 0x%08x\n", reg_off, value);
    return sizeof(value);
}

static ssize_t axi_mult_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
    struct axi_mult_dev *d = filp->private_data;
    u32 value;
    u32 reg_off = *off;

    if (len < sizeof(value))
        return -EINVAL;

    if (!reg_is_valid(reg_off))
        return -EINVAL;

    if (!reg_is_writable(reg_off))
        return -EACCES;

    if (copy_from_user(&value, buf, sizeof(value)))
        return -EFAULT;

    mutex_lock(&d->lock);
    writel(value, d->base + reg_off);
    mutex_unlock(&d->lock);

    *off += sizeof(value);
    dev_info(d->dev, "wrote reg 0x%02x = 0x%08x\n", reg_off, value);
    return sizeof(value);
}

static int axi_release(struct inode *inode, struct file *filp)
{
    return 0;
}

static const struct file_operations axi_fops = {
    .owner = THIS_MODULE,
    .open = axi_open,
    .release = axi_release,
    .read = axi_mult_read,
    .write = axi_mult_write,
    .llseek = axi_llseek,
};

static int axi_mult_probe(struct platform_device *pdev)
{
    struct axi_mult_dev *d;
    struct resource *res;
    int ret;

    dev_info(&pdev->dev, "probe called\n");

    d = devm_kzalloc(&pdev->dev, sizeof(*d), GFP_KERNEL);
    if (!d)
        return -ENOMEM;

    d->dev = &pdev->dev;
    mutex_init(&d->lock);

    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    d->base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(d->base))
        return PTR_ERR(d->base);

    ret = alloc_chrdev_region(&d->devno, 0, 1, DEV_NAME);
    if (ret)
        return ret;

    cdev_init(&d->cdev, &axi_fops);
    d->cdev.owner = THIS_MODULE;

    ret = cdev_add(&d->cdev, d->devno, 1);
    if (ret)
        goto err_chrdev;

    d->class = class_create(THIS_MODULE, DEV_NAME);
    if (IS_ERR(d->class)) {
        ret = PTR_ERR(d->class);
        goto err_cdev;
    }

    d->device = device_create(d->class, &pdev->dev, d->devno, d, DEV_NAME);
    if (IS_ERR(d->device)) {
        ret = PTR_ERR(d->device);
        goto err_class;
    }

    platform_set_drvdata(pdev, d);

    dev_info(&pdev->dev, "/dev/%s created\n", DEV_NAME);
    return 0;

err_class:
    class_destroy(d->class);
err_cdev:
    cdev_del(&d->cdev);
err_chrdev:
    unregister_chrdev_region(d->devno, 1);
    return ret;
}

static int axi_mult_remove(struct platform_device *pdev)
{
    struct axi_mult_dev *d = platform_get_drvdata(pdev);

    dev_info(&pdev->dev, "remove called\n");

    device_destroy(d->class, d->devno);
    class_destroy(d->class);
    cdev_del(&d->cdev);
    unregister_chrdev_region(d->devno, 1);

    return 0;
}

static const struct of_device_id axi_mult_of_match[] = {
    { .compatible = "BMSTU,axi-multiplier-1.0" },
    { }
};
MODULE_DEVICE_TABLE(of, axi_mult_of_match);

static struct platform_driver axi_mult_driver = {
    .probe = axi_mult_probe,
    .remove = axi_mult_remove,
    .driver = {
        .name = DRV_NAME,
        .of_match_table = axi_mult_of_match,
    },
};

static int __init axi_mult_init(void)
{
    pr_info("axi_multiplier: module loaded\n");
    return platform_driver_register(&axi_mult_driver);
}

static void __exit axi_mult_exit(void)
{
    pr_info("axi_multiplier: module unloaded\n");
    platform_driver_unregister(&axi_mult_driver);
}

module_init(axi_mult_init);
module_exit(axi_mult_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Perplexity");
MODULE_DESCRIPTION("AXI multiplier driver");
