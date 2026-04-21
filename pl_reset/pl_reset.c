// pl_reset.c
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>
#include <linux/gpio/consumer.h>
#include <linux/delay.h>
#include <linux/sysfs.h>

struct pl_reset_dev {
    struct gpio_desc *reset_gpiod;
};

static ssize_t reset_store(struct device *dev,
                           struct device_attribute *attr,
                           const char *buf,
                           size_t count)
{
    struct pl_reset_dev *priv = dev_get_drvdata(dev);

    if (!priv || !priv->reset_gpiod)
        return -EINVAL;

    gpiod_set_value_cansleep(priv->reset_gpiod, 0); /* assert resetn */
    msleep(10);
    gpiod_set_value_cansleep(priv->reset_gpiod, 1); /* deassert resetn */

    dev_info(dev, "PL reset pulsed on GPIO 173\n");
    return count;
}

static DEVICE_ATTR_WO(reset);

static int pl_reset_probe(struct platform_device *pdev)
{
    struct pl_reset_dev *priv;
    int ret;

    priv = devm_kzalloc(&pdev->dev, sizeof(*priv), GFP_KERNEL);
    if (!priv)
        return -ENOMEM;

    platform_set_drvdata(pdev, priv);

    priv->reset_gpiod = devm_gpiod_get(&pdev->dev, "reset", GPIOD_OUT_LOW);
    if (IS_ERR(priv->reset_gpiod))
        return PTR_ERR(priv->reset_gpiod);

    ret = device_create_file(&pdev->dev, &dev_attr_reset);
    if (ret)
        return ret;

    dev_info(&pdev->dev, "PL GPIO reset helper loaded\n");
    return 0;
}

static int pl_reset_remove(struct platform_device *pdev)
{
    device_remove_file(&pdev->dev, &dev_attr_reset);
    return 0;
}

static const struct of_device_id pl_reset_of_match[] = {
    { .compatible = "BMSTU,pl-reset" },
    { }
};
MODULE_DEVICE_TABLE(of, pl_reset_of_match);

static struct platform_driver pl_reset_driver = {
    .probe  = pl_reset_probe,
    .remove = pl_reset_remove,
    .driver = {
        .name = "pl-reset",
        .of_match_table = pl_reset_of_match,
    },
};

module_platform_driver(pl_reset_driver);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("PL reset helper using GPIO");