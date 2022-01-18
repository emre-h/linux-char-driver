#ifndef EMRE_H_
#define EMRE_H_

#include <linux/uaccess.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/export.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h> 
#include <linux/kdev_t.h>
#include <linux/device.h>

struct class *pClass;
dev_t devNo;

static int major_number = 0;

static const char module_message[] = "emre-module-message: calisiyor!\n\0";
static const char driver_message_prefix[] = "emre-driver:";
static const char device_name[] = "emre";

static ssize_t message_size = sizeof(module_message);

int register_device(void);
void unregister_device(void);

static ssize_t device_file_read(
    struct file *file_ptr,
    char __user *user_buffer,
    size_t count,
    loff_t *position
);

#endif