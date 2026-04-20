// pl_reset.c
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>
#include <linux/reset.h>
#include <linux/delay.h>
#include <linux/sysfs.h>

struct pl_reset_dev {
    struct reset_control *rstc;
};

static ssize_t reset_store(struct device *dev,
                           struct device_attribute *attr,
                           const char *buf,
                           size_t count)
{
    struct pl_reset_dev *priv = dev_get_drvdata(dev);
    int ret;

    if (!priv || !priv->rstc)
        return -EINVAL;

    ret = reset_control_assert(priv->rstc);
    if (ret)
        return ret;

    msleep(10);

    ret = reset_control_deassert(priv->rstc);
    if (ret)
        return ret;

    dev_info(dev, "PL reset pulsed by sysfs\n");
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

    priv->rstc = devm_reset_control_get_exclusive(&pdev->dev, "pl_resetn0");
    if (IS_ERR(priv->rstc))
        return PTR_ERR(priv->rstc);

    platform_set_drvdata(pdev, priv);

    ret = device_create_file(&pdev->dev, &dev_attr_reset);
    if (ret)
        return ret;

    dev_info(&pdev->dev, "PL reset helper loaded\n");
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
MODULE_DESCRIPTION("PL reset helper using Linux reset controller");
