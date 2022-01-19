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

#define BUFFER_SIZE 1024

static char device_buffer[BUFFER_SIZE];

struct class *pClass;
dev_t devNo;

static int bytes_written = 0;
static int major_number = 0;

static char* module_message = "emre-module-message: calisiyor!\n\0";
static const char driver_message_prefix[] = "emre-driver:";
static const char device_name[] = "emre";

static ssize_t message_size = sizeof(module_message);

int register_device(void);
void unregister_device(void);
void logstr(const char *logstr_msg);
void logstrd(const char *logstr_msg, int value);

static ssize_t device_file_read(struct file *file_ptr, char __user *user_buffer, size_t count, loff_t *position);
static ssize_t device_file_write(struct file *, const char *, size_t, loff_t *);

static int device_file_release(struct inode *inode, struct file *file);
static int device_file_open(struct inode *inode, struct file *file);

static long device_file_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

#endif