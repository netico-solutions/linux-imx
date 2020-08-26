/* si705x.c
 *
 * Copyright (C) 2020 Nemanja Stefanovic <nemanja.stef92@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/* Driver for the Silicon Labs SI705x I2C Temperature sensors
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/err.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define DEVICE_NAME	"si705x"
/*Chip address*/
#define SI705X_I2C_ADDR 0x40

/*Chip commands*/
#define SI705X_MEAS_TEMP_HOLD       0xE3
#define SI705X_MEAS_TEMP_NO_HOLD    0xF3

static dev_t dev;
static struct class *dev_class;
struct cdev cdev;

struct si705x {
    struct i2c_client   *client;
    u8[2]             temp_reading;
};

struct si705x *sensor;

static ssize_t si705x_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
    printk(KERN_INFO "Reading temperature\n");
    u16 raw_temp = i2c_smbus_read_word(sensor->i2c_client, SI705X_MEAS_TEMP_NO_HOLD);

    sensor->temp_reading[0] = raw_temp & 0xFF;
    sensor->temp_reading[1] = (raw_temp >> 8) & 0xFF;

    copy_to_user(buf, sensor->temp_reading, 2);
    return 0;
};

static ssize_t si705x_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
    printk(KERN_INFO "Writing to SI705X\n");
    return len;
};

static int si705x_open(struct inode *inode, struct file *file)
{
        printk(KERN_INFO "SI705X opened\n");
        return 0;
};

static int si705x_release(struct inode *inode, struct file *file)
{
        printk(KERN_INFO "SI705X closed\n");
        return 0;
};

static struct file_operations fops =
{
	.owner          = THIS_MODULE,
	.read           = si705x_read,
	.write          = si705x_write,
    .open          = si705x_open,
    .release        = si705x_release,
};
static const struct i2c_device_id si705x_idtable[] = {
    { "si7050", 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, si705x_idtable);

static int si705x_probe(struct i2c_client *i2c_client, const struct i2c_device_id *id)
{
    printk(KERN_INFO "Probing SI705X\n");
    sensor = (struct si705x *)kmalloc(sizeof(struct si705x), GFP_KERNEL);
    i2c_client->addr = SI705X_I2C_ADDR;
    sensor->i2c_client = i2c_client;

    int i;
    for(i = 0; i < 2; i++) {
        sensor->readings[i] = 0;
    }

    return 0;

};

static int si705x_remove(struct i2c_client *client)
{
    kfree(sensor);
    return 0;
};

static struct i2c_driver si705x_driver = {
    .driver = {
        .name = "si705x",
		.owner = THIS_MODULE
    },

    .id_table           = si705x_idtable,
    .probe              = si705x_probe,
    .remove             = si705x_remove
};

static int __init si705x_driver_init(void)
{

    if((alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME)) <0){
        printk(KERN_INFO "Cannot allocate major number\n");
        return -1;
    }
    printk(KERN_INFO "Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));

    cdev_init(&cdev,&fops);
    if((cdev_add(&cdev,dev,1)) < 0){
        printk(KERN_INFO "Cannot add the device to the system\n");
        unregister_chrdev_region(dev,1);
        return -1;
    }

    if((dev_class = class_create(THIS_MODULE,DEVICE_NAME)) == NULL){
        printk(KERN_INFO "Cannot create the struct class\n");
        unregister_chrdev_region(dev,1);
        return -1;
    }

    if((device_create(dev_class,NULL,dev,NULL,DEVICE_NAME)) == NULL){
        printk(KERN_INFO "Cannot create the Device 1\n");
        unregister_chrdev_region(dev,1);
        return -1;;
    }
    
    i2c_add_driver(&htu21df_driver);
    printk(KERN_INFO "Device Driver SI705X Init\n");

    return 0;

}

void __exit si705x_driver_exit(void)
{
    device_destroy(dev_class,dev);
    class_destroy(dev_class);
    cdev_del(&cdev);
    unregister_chrdev_region(dev, 1);
    i2c_del_driver(&si705x_driver);
    printk(KERN_INFO "SI705X REMOVED!\n");
}

module_init(si705x_driver_init);
module_exit(si705x_driver_exit);

MODULE_AUTHOR("n-stef");
MODULE_DESCRIPTION("Driver for SI7050 Temp Sensor");

MODULE_LICENSE("GPL");

