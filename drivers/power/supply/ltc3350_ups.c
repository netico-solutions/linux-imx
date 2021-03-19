#include <linux/of.h>
#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/printk.h>

const void *ltc3350_data;

static int ltc3350_probe(static struct i2c_client *client,
						 const struct i2c_device_id *id) {
	pr_info("ltc3350: Inside ltc3350 probe function");
	return 0;
};

static int ltc3350_remove() {
	pr_info("ltc3350: Inside ltc3350 remove function");
	return 0;
};

static const struct i2c_device_id ltc3350_i2c_id[] = {
		{"ltc3350_ups", 0},
};
MODULE_DEVICE_TABLE(i2c, ltc3350_i2c_id);

static const struct of_device_id ltc3350_i2c_of_match[] = {
		{.compatible = "ltc,ltc3350-i2c"},
};
MODULE_DEVICE_TABLE(of, ltc3350_i2c_of_match);

static struct i2c_driver ltc3350_driver = {
		.probe			= ltc3350_probe,
		.remove			= ltc3350_remove,
		.id_table		= ltc3350_i2c_id,
		.driver			= {
				.name = "ltc3350_ups",
				.of_match_table = of_match_ptr(ltc3350_i2c_of_match),
		},
};

module_i2c_driver(ltc3350_driver);
