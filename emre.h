#ifndef EMRE_H_
#define EMRE_H_

#include <linux/uaccess.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/export.h>
#include <linux/kdev_t.h>
#include <linux/device.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/wait.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/fs.h>

#define TIMER_INTERVAL 5000
#define BUFFER_SIZE 1024

static char device_buffer[BUFFER_SIZE];

static struct timer_list timer;

struct class *pClass;

dev_t devNo;

static int bytes_written = 0;
static int major_number = 0;

static char* module_message = "emre-module-message: calisiyor!\n\0";
static const char driver_message_prefix[] = "emre-driver:";
static const char device_name[] = "emre";

static ssize_t message_size = sizeof(module_message);

int register_device(void);

void logstrd(const char *logstr_msg, int value);
void timer_callback(struct timer_list * data);
void logstr(const char *logstr_msg);
void unregister_device(void);
void init_timer(void);

static ssize_t device_file_read(struct file *file_ptr, char __user *user_buffer, size_t count, loff_t *position);
static ssize_t device_file_write(struct file *, const char *, size_t, loff_t *);

static unsigned int etx_poll(struct file *filp, struct poll_table_struct *wait);

static int device_file_release(struct inode *inode, struct file *file);
static int device_file_open(struct inode *inode, struct file *file);

static long device_file_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

#endif