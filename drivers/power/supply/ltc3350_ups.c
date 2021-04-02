#include <linux/of.h>
#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/kernel.h>
#include <linux/device.h>


struct ltc3350 {
	struct i2c_client *client;
	struct device *dev;
	struct client_ops *ltc3350_client_ops;
};

struct client_ops {
	int (*read_value)(struct i2c_client *client, u8 reg);
	int (*write_value)(struct i2c_client *client, u8 reg, u16 value);
};

const void *ltc3350_data;
//static unsigned long ltc3350_private_data;

static int ltc3350_read_value(struct i2c_client *client, u8 reg)
{
	return i2c_smbus_read_byte_data(client, reg);
}

static int ltc3350_write_value(struct i2c_client *client, u8 reg, u16 value)
{
	return i2c_smbus_write_byte_data(client, reg, value);
}

static int ltc3350_probe(struct i2c_client *client,
						 const struct i2c_device_id *id) {
	struct ltc3350 *ltc3350;
	u8 reg;
	//u16 value__to_write;
	u8 read_value;
	int ret = 0;

	ltc3350 = devm_kzalloc(&client->dev, sizeof(struct ltc3350), GFP_KERNEL);
	if (ltc3350 == NULL){
		return -ENOMEM;
	}

	i2c_set_clientdata(client, ltc3350);

	ltc3350->client = client;
	ltc3350->ltc3350_client_ops->read_value = ltc3350_read_value;
	ltc3350->ltc3350_client_ops->write_value = ltc3350_write_value;

	reg = 0x2A;
	read_value = ltc3350->ltc3350_client_ops->read_value(ltc3350->client, reg);
	if (read_value){
		pr_info("ltc3350: Read value for temp: %X", read_value);
	}
	else {
		pr_info("ltc3350: Unable to read value for temp!!");
		ret = -1;
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
