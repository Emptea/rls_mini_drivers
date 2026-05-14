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

#define DRV_NAME "axi_multiplier"
#define DEV_NAME "axi_multiplier"

#define REG_OFFSET_IP_VER 0x00
#define REG_OFFSET_KILL 0x04
#define REG_OFFSET_TEST_POINT 0x08
#define REG_OFFSET_CHANNEL 0x0C
#define REG_OFFSET_MULT0 0x10
#define REG_OFFSET_MULT1 0x14

#define REG_MAX_OFFSET REG_OFFSET_MULT1
#define REG_SIZE sizeof(u32)

struct axi_mult_dev
{
    void __iomem *base;
    struct mutex lock;
};

static dev_t devno;
static struct class *axi_class;
static struct cdev axi_cdev;
static struct device *axi_device;
static struct axi_mult_dev *gdev;

static bool reg_is_valid(u32 off)
{
    switch (off)
    {
    case REG_OFFSET_IP_VER:
    case REG_OFFSET_KILL:
    case REG_OFFSET_TEST_POINT:
    case REG_OFFSET_CHANNEL:
    case REG_OFFSET_MULT0:
    case REG_OFFSET_MULT1:
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

    switch (whence)
    {
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

static ssize_t axi_mult_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
    u32 value;
    u32 reg_off = *off;

    if (len < sizeof(value))
        return -EINVAL;

    if (!reg_is_valid(reg_off))
        return -EINVAL;

    mutex_lock(&gdev->lock);
    value = readl(gdev->base + reg_off);
    mutex_unlock(&gdev->lock);

    if (copy_to_user(buf, &value, sizeof(value)))
        return -EFAULT;

    *off += sizeof(value);
    pr_info("axi_multiplier: read reg 0x%02x = 0x%08x\n", reg_off, value);
    return sizeof(value);
}

static ssize_t axi_mult_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
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

    mutex_lock(&gdev->lock);
    writel(value, gdev->base + reg_off);
    mutex_unlock(&gdev->lock);

    *off += sizeof(value);
    pr_info("axi_multiplier: wrote reg 0x%02x = 0x%08x\n", reg_off, value);
    return sizeof(value);
}

static const struct file_operations axi_fops = {
    .owner = THIS_MODULE,
    .read = axi_mult_read,
    .write = axi_mult_write,
    .llseek = axi_llseek,
};

static int axi_mult_probe(struct platform_device *pdev)
{
    struct resource *res;
    int ret;

    pr_info("axi_multiplier: probe called\n");

    gdev = devm_kzalloc(&pdev->dev, sizeof(*gdev), GFP_KERNEL);
    if (!gdev)
        return -ENOMEM;

    mutex_init(&gdev->lock);

    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    gdev->base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(gdev->base))
        return PTR_ERR(gdev->base);

    ret = alloc_chrdev_region(&devno, 0, 1, DEV_NAME);
    if (ret)
        return ret;

    cdev_init(&axi_cdev, &axi_fops);
    axi_cdev.owner = THIS_MODULE;

    ret = cdev_add(&axi_cdev, devno, 1);
    if (ret)
        goto err_chrdev;

    axi_class = class_create(THIS_MODULE, DEV_NAME);
    if (IS_ERR(axi_class))
    {
        ret = PTR_ERR(axi_class);
        goto err_cdev;
    }

    axi_device = device_create(axi_class, NULL, devno, NULL, DEV_NAME);
    if (IS_ERR(axi_device))
    {
        ret = PTR_ERR(axi_device);
        goto err_class;
    }

    pr_info("axi_multiplier: /dev/%s created\n", DEV_NAME);
    return 0;

err_class:
    class_destroy(axi_class);
err_cdev:
    cdev_del(&axi_cdev);
err_chrdev:
    unregister_chrdev_region(devno, 1);
    return ret;
}

static int axi_mult_remove(struct platform_device *pdev)
{
    pr_info("axi_multiplier: remove called\n");
    device_destroy(axi_class, devno);
    class_destroy(axi_class);
    cdev_del(&axi_cdev);
    unregister_chrdev_region(devno, 1);
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
