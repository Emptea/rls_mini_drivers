#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>
#include <linux/of_address.h>
#include <linux/io.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/printk.h>

#define DRV_NAME "axi_multiplier"
#define DEV_NAME "axi_multiplier"
#define MULTIPLIER_S00_AXI_SLV_REG0_OFFSET 0x00

struct axi_mult_dev {
    void __iomem *base;
};

static dev_t devno;
static struct class *axi_class;
static struct cdev axi_cdev;
static struct device *axi_device;
static struct axi_mult_dev *gdev;

static ssize_t axi_mult_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
    u32 value;

    if (len < sizeof(value))
        return -EINVAL;
    if (copy_from_user(&value, buf, sizeof(value)))
        return -EFAULT;

    writel(value & 0xFFFF, gdev->base + MULTIPLIER_S00_AXI_SLV_REG0_OFFSET);
    pr_info("axi_multiplier: wrote 0x%08x\n", value & 0xFFFF);
    return sizeof(value);
}

static ssize_t axi_mult_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
    u32 value;

    if (len < sizeof(value))
        return -EINVAL;

    value = readl(gdev->base + MULTIPLIER_S00_AXI_SLV_REG0_OFFSET);

    if (copy_to_user(buf, &value, sizeof(value)))
        return -EFAULT;

    pr_info("axi_multiplier: read 0x%08x\n", value);
    return sizeof(value);
}

static const struct file_operations axi_fops = {
    .owner = THIS_MODULE,
    .read = axi_mult_read,
    .write = axi_mult_write,
};

static int axi_mult_probe(struct platform_device *pdev)
{
    struct resource *res;
    int ret;

    pr_info("axi_multiplier: probe called\n");

    gdev = devm_kzalloc(&pdev->dev, sizeof(*gdev), GFP_KERNEL);
    if (!gdev)
        return -ENOMEM;

    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    gdev->base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(gdev->base))
        return PTR_ERR(gdev->base);

    ret = alloc_chrdev_region(&devno, 0, 1, DEV_NAME);
    if (ret)
        return ret;

    cdev_init(&axi_cdev, &axi_fops);
    ret = cdev_add(&axi_cdev, devno, 1);
    if (ret)
        goto err_chrdev;

    axi_class = class_create(THIS_MODULE, DEV_NAME);
    if (IS_ERR(axi_class)) {
        ret = PTR_ERR(axi_class);
        goto err_cdev;
    }

    axi_device = device_create(axi_class, NULL, devno, NULL, DEV_NAME);
    if (IS_ERR(axi_device)) {
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
