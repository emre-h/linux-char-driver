#include "linux/slab.h"
#include "emre.h"

static int __init init(void) {
    logstr("aktiflestirildi");
    register_device();
    return 0;
}

static void __exit cleanup(void) {
    logstr("modul temizleniyor");
    unregister_device();
}

static struct file_operations driver_fops = {
    .owner          = THIS_MODULE,
    .unlocked_ioctl = device_file_ioctl,
    .open           = device_file_open,
    .release        = device_file_release,
    .read           = device_file_read,
    .write          = device_file_write,
};

int register_device(void) {
    uint8_t result = 0;

    logstr("register_device() cagrildi.");

    result = register_chrdev(0, device_name, &driver_fops);
    
    if(result < 0) {
        logstr("cihaz kaydi olusturulurken hata");
        return result;
    }
    
    major_number = result;

    logstrd("cihaz kaydedildi. major number =", major_number);

    struct device *pDev;

    // major ve minor numaraların 32 bit kombinasyonlu hali
    devNo = MKDEV(major_number, 0); 
    
    // /dev/emre olusturmak icin /sys/class/emre olusturuluyor
    pClass = class_create(THIS_MODULE, device_name);

    if (IS_ERR(pClass)) {
        logstr("class olusturulurken hata");
        unregister_chrdev_region(devNo, 1);
        return -1;
    }

    if (IS_ERR(pDev = device_create(pClass, NULL, devNo, NULL, device_name))) {
        logstr("/dev/emre olusturulurken hata");
        
        class_destroy(pClass);
        unregister_chrdev_region(devNo, 1);
        return -1;
    }

    return 0;
}

void unregister_device(void) {
    logstr("unregister_device() cagrildi");

    if(major_number != 0) {
        device_destroy(pClass, devNo);
        class_unregister(pClass);
        class_destroy(pClass);
        unregister_chrdev_region(devNo, 1);
        unregister_chrdev(major_number, device_name);
    }
}

static ssize_t device_file_read(struct file *file_ptr, char __user *user_buffer, size_t count, loff_t *position) {
    logstr("okunan veri:");
    logstr(module_message);
    logstrd("cihaz okuma offseti =", (int) *position);
    logstrd("okunan byte boyutu =", count);

    message_size = strlen(module_message);

    if(*position >= message_size)
        return 0;

    if(*position + count > message_size)
        count = message_size - *position; //count boyutu asarsa max olan verilecek

    if(copy_to_user(user_buffer, module_message + *position, bytes_written) != 0)
        return -EFAULT;

    *position += count; //ileri kaydir
    return count;
}

static ssize_t device_file_write(struct file *file_ptr, const char __user *user_buffer, size_t count, loff_t *position) {
    int maxbytes;
    int bytes_to_write;

    maxbytes = BUFFER_SIZE - *position;

    bytes_to_write = maxbytes > count ? count : maxbytes;

    bytes_written = bytes_to_write - copy_from_user(device_buffer + *position, user_buffer, bytes_to_write);

    *position += bytes_written;
    
    logstr("cihaza yazildi");
    logstr("veri:");
    logstr(user_buffer);

    module_message = (char*) kmalloc(bytes_written, GFP_KERNEL);

    if (copy_from_user(module_message, user_buffer, bytes_written)) {
        logstr("success");
    }

    return bytes_written;
}

static long device_file_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    logstr("device ioctl fonksiyonu");
    return 0;
}

static int device_file_open(struct inode *inode, struct file *file) {
    logstr("cihaz açık");
    return 0;
}

static int device_file_release(struct inode *inode, struct file *file) {
    logstr("cihaz kapalı\n------");
    return 0;
}

void logstr(const char *logstr_msg) {
    printk(KERN_INFO "%s %s\n", driver_message_prefix, logstr_msg);    
}

void logstrd(const char *logstr_msg, int value) {
    printk(KERN_INFO "%s %s %d\n", driver_message_prefix, logstr_msg, value);    
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Emre Harbutoglu");
MODULE_DESCRIPTION("Device driver module");

module_init(init);
module_exit(cleanup);