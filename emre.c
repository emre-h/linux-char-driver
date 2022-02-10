#include "emre.h"

#include <linux/semaphore.h>
#include <linux/poll.h>

struct semaphore sem;

static bool can_write = false;
static bool can_read = false;

DECLARE_WAIT_QUEUE_HEAD(wait_queue_etx_data);

static struct file_operations driver_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = device_file_ioctl,
    .open = device_file_open,
    .release = device_file_release,
    .read = device_file_read,
    .write = device_file_write,
    .poll = poll_call,
};

static int __init init(void)
{
    logstr("aktiflestirildi");
    register_device();
    sema_init(&sem, 1);
    init_timer();

    return 0;
}

static void __exit cleanup(void)
{
    del_timer(&timer);
    unregister_device();
    logstr("modul temizleniyor");
}

int register_device(void)
{
    uint8_t result = 0;

    logstr("register_device() cagrildi.");

    result = register_chrdev(0, device_name, &driver_fops);

    if (result < 0)
    {
        logstr("cihaz kaydi olusturulurken hata");
        return result;
    }

    major_number = result;

    logstrd("cihaz kaydedildi. major number =", major_number);

    struct device *pDev;

    /* major ve minor numaraların 32 bit kombinasyonlu hali */
    devNo = MKDEV(major_number, 0);

    /* /dev/emre olusturmak icin /sys/class/emre olusturuluyor */
    pClass = class_create(THIS_MODULE, device_name);

    if (IS_ERR(pClass))
    {
        logstr("class olusturulurken hata");
        unregister_chrdev_region(devNo, 1);
        return -1;
    }

    if (IS_ERR(pDev = device_create(pClass, NULL, devNo, NULL, device_name)))
    {
        logstr("/dev/emre olusturulurken hata");

        class_destroy(pClass);
        unregister_chrdev_region(devNo, 1);
        return -1;
    }

    return 0;
}

void timer_callback(struct timer_list *data)
{
    wake_up(&wait_queue_etx_data);
    logstr("timer calisiyor");
    mod_timer(&timer, jiffies + msecs_to_jiffies(TIMER_INTERVAL));
    can_read = true;
}

void init_timer(void)
{
    timer_setup(&timer, (timer_callback), 0);
    mod_timer(&timer, jiffies + msecs_to_jiffies(TIMER_INTERVAL));
}

void unregister_device(void)
{
    logstr("unregister_device() cagrildi");

    if (major_number != 0)
    {
        device_destroy(pClass, devNo);
        class_unregister(pClass);
        class_destroy(pClass);
        unregister_chrdev_region(devNo, 1);
        unregister_chrdev(major_number, device_name);
    }
}

static ssize_t device_file_read(struct file *file_ptr, char __user *user_buffer,
                                size_t count, loff_t *position)
{
    if (can_read)
    {
        logstr("okunan veri:");
        logstr(module_message);
        logstrd("cihaz okuma offseti =", (int)*position);
        logstrd("okunan byte boyutu =", count);

        message_size = strlen(module_message);

        if (*position >= message_size)
            return 0;

        if (*position + count > message_size)
            count = message_size -
                    *position; /* count boyutu asarsa max olan verilecek */

        if (copy_to_user(user_buffer, module_message + *position,
                         bytes_written) != 0)
            return -EFAULT;

        *position += count; /* ileri kaydir */
        return count;
    }
    else
    {
        return 0;
    }
}

static ssize_t device_file_write(struct file *file_ptr,
                                 const char __user *user_buffer, size_t count,
                                 loff_t *position)
{
    int maxbytes;
    int bytes_to_write;

    if (can_write)
    {
        maxbytes = BUFFER_SIZE - *position;

        bytes_to_write = maxbytes > count ? count : maxbytes;

        bytes_written =
            bytes_to_write - copy_from_user(device_buffer + *position,
                                            user_buffer, bytes_to_write);

        *position += bytes_written;

        logstr("cihaza yazildi");
        logstr("veri:");
        logstr(user_buffer);

        module_message = (char *)kmalloc(bytes_written, GFP_KERNEL);

        if (copy_from_user(module_message, user_buffer, bytes_written))
        {
            logstr("success");
        }
    }

    return bytes_written;
}

static unsigned int poll_call(struct file *filp,
                              struct poll_table_struct *wait)
{
    __poll_t mask = 0;

    poll_wait(filp, &wait_queue_etx_data, wait);

    if (can_read)
    {
        can_read = false;
        mask |= (POLLIN | POLLRDNORM);
        pr_info("POLLIN\n");
    }

    if (can_write)
    {
        can_write = false;
        mask |= (POLLOUT | POLLWRNORM);
        pr_info("POLLOUT\n");
    }

    return mask;
}

static long device_file_ioctl(struct file *file, unsigned int cmd,
                              unsigned long arg)
{
    logstr("device ioctl fonksiyonu");
    return 0;
}

static int device_file_open(struct inode *inode, struct file *file)
{
    logstr("cihaz açık");
    if (down_interruptible(&sem))
    {
        printk(KERN_INFO "semaphore tutulamıyor");
        return -1;
    }
    else
    {
        printk(KERN_INFO "semaphore tutuldu");
    }
    return 0;
}

static int device_file_release(struct inode *inode, struct file *file)
{
    logstr("cihaz kapalı\n------");
    up(&sem);
    return 0;
}

int hold(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    int len = 0;
    down_interruptible(&sem);
    printk(KERN_INFO "semaphore tutuluyor");
    return len;
}

void logstr(const char *logstr_msg)
{
    printk(KERN_INFO "%s %s\n", driver_message_prefix, logstr_msg);
}

void logstrd(const char *logstr_msg, int value)
{
    printk(KERN_INFO "%s %s %d\n", driver_message_prefix, logstr_msg, value);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Emre Harbutoglu");
MODULE_DESCRIPTION("Device driver module");

module_init(init);
module_exit(cleanup);