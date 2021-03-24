#include <linux/of.h>
#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/kernel.h>
#include <linux/device.h>


struct ltc3350 {
	struct device *dev;
};

const void *ltc3350_data;
static unsigned long ltc3350_private_data;

static int ltc3350_probe(struct i2c_client *client,
						 const struct i2c_device_id *id) {
	struct ltc3350 *ltc3350;
	int ret = 0;

	ltc3350 = devm_kzalloc(&client->dev, sizeof(struct ltc3350), GFP_KERNEL);
	if (ltc3350 == NULL){
		return -ENOMEM;
	}

	pr_info("ltc3350: Inside ltc3350 probe function");
	return ret;
};

static int ltc3350_remove(struct i2c_client *client) {
	pr_info("ltc3350: Inside ltc3350 remove function");
	return 0;
};

static const struct i2c_device_id ltc3350_i2c_id[] = {
		{"ltc3350_ups", 0},
		{},
};
MODULE_DEVICE_TABLE(i2c, ltc3350_i2c_id);

static const struct of_device_id ltc3350_i2c_of_match[] = {
		{.compatible = "ltc,ltc3350-i2c",},
		{},
};
MODULE_DEVICE_TABLE(of, ltc3350_i2c_of_match);

static struct i2c_driver ltc3350_driver = {
		.probe			= ltc3350_probe,
		.remove			= ltc3350_remove,
		.id_table		= ltc3350_i2c_id,
		.driver			= {
				.name = "ltc3350_ups",
				.of_match_table = ltc3350_i2c_of_match,
		},
};

/*static int __init ltc3350_init(void)
{
	return i2c_add_driver(&ltc3350_driver);
}
module_init(ltc3350_init);

static void __exit ltc3350_cleanup(void)
{
	i2c_del_driver(&ltc3350_driver);
}
module_exit(ltc3350_cleanup);*/

module_i2c_driver(ltc3350_driver);

/* Substitute your own name and email address */
MODULE_AUTHOR("Lazar Sladojevic <lazar.sladojevic@netico-group.com>");
MODULE_DESCRIPTION("Driver for ltc3350 over i2c.");

/* a few non-GPL license types are also allowed */
MODULE_LICENSE("GPL");
