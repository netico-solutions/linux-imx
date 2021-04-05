#include <linux/of.h>
#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/kernel.h>
#include <linux/device.h>


struct ltc3350 {
	struct i2c_client *client;
	//struct device *dev;
	//struct client_ops *ltc3350_client_ops;
};
//
//struct client_ops {
//	int (*read_value)(const struct i2c_client *client, u8 reg);
//	int (*write_value)(const struct i2c_client *client, u8 reg, u16 value);
//};

//static const void *ltc3350_data;
//static unsigned long ltc3350_private_data;

static s32 ltc3350_read_value(const struct i2c_client *client, u8 reg)
{
	return i2c_smbus_read_byte_data(client, reg);
}

static s32 ltc3350_read_word_value(const struct i2c_client *client, u8 reg)
{
	return i2c_smbus_read_word_data(client, reg);
}

static s32 ltc3350_read_block_value(const struct i2c_client *client, u8 reg, u8 len, u8 *values)
{
	return i2c_smbus_read_i2c_block_data(client, reg, len, values);
}

//static int ltc3350_write_value(const struct i2c_client *client, u8 reg, u16 value)
//{
//	return i2c_smbus_write_byte_data(client, reg, value);
//}

static int ltc3350_probe(struct i2c_client *client,
						 const struct i2c_device_id *id) {
	struct ltc3350 *ltc3350;
	u8 reg;
	//u16 value__to_write;
	u8 read_value;
	u16 word_value;
	s16 converted_data;
	u8 block_values[2];
//	float temp;
	int i;
	int ret = 0;

	ltc3350 = devm_kzalloc(&client->dev, sizeof(struct ltc3350), GFP_KERNEL);
	if (ltc3350 == NULL){
		return -ENOMEM;
	}

	i2c_set_clientdata(client, ltc3350);

	ltc3350->client = client;
//	ltc3350->ltc3350_client_ops->read_value = ltc3350_read_value;
//	ltc3350->ltc3350_client_ops->write_value = ltc3350_write_value;

	reg = 0x2A;
	read_value = ltc3350_read_value(ltc3350->client, reg);
	if (read_value){
		pr_info("ltc3350: Read value for temp: 0x%X\n", read_value);
	}
	else {
		pr_info("ltc3350: Unable to read value for temp!!\n");
		ret = -1;
	}
	read_value = ltc3350_read_value(ltc3350->client, reg);
	if (read_value){
		pr_info("ltc3350: Read value for temp: 0x%d\n", read_value);
	}
	else {
		pr_info("ltc3350: Unable to read value for temp!!\n");
		ret = -1;
	}

	word_value = ltc3350_read_word_value(ltc3350->client, reg);
	if (read_value){
		pr_info("ltc3350: Read value for temp: 0x%X\n", word_value);
	}
	else {
		pr_info("ltc3350: Unable to read value for temp!!\n");
		ret = -1;
	}

	ret = ltc3350_read_block_value(ltc3350->client, 0x2a, sizeof(block_values), block_values);
	pr_info("ltc3350: ret block_value: %d\n", ret);
	if (ret == sizeof(block_values)){
		for (i=0; i<2; i++) {
			pr_info("ltc3350: Read block_value[%d]: %d\n", i, block_values[i]);
		}

	}
	else {
		pr_info("ltc3350: Unable to read block value for temp!!\n");
		ret = -1;
	}

	converted_data = (word_value & (0xff)) + (word_value & (0xff << 8)) * 256;
	pr_info("ltc3350: Converted value for temp: %X\n", converted_data);
//	temp = 0.028 * converted_data - 251.4;
//	pr_info("ltc3350: Actual temp: %f\n", temp);

	pr_info("ltc3350: Inside ltc3350 probe function\n");
	return ret;
};

static int ltc3350_remove(struct i2c_client *client) {
	pr_info("ltc3350: Inside ltc3350 remove function\n");
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
