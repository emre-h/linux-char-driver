#include "emre.h"

static int __init init(void) {
    printk(KERN_INFO "%s aktif\n", driver_message_prefix);
    register_device();
    return 0;
}

static void __exit cleanup(void) {
    printk(KERN_INFO "%s modul temizleniyor.\n", driver_message_prefix);
    unregister_device();
}

static struct file_operations driver_fops = {
    .owner = THIS_MODULE,
    .read = device_file_read,
};

int register_device(void) {
    uint8_t result = 0;

    printk(KERN_NOTICE "%s register_device() cagrildi.\n", driver_message_prefix);
    result = register_chrdev(0, device_name, &driver_fops);
    
    if(result < 0) {
        printk(KERN_WARNING "%s  cihaz kaydi olusturulurken hata = %i\n", driver_message_prefix, result);
        return result;
    }
    
    major_number = result;
    printk(KERN_NOTICE "%s cihaz kaydedildi. major number = %i", driver_message_prefix, major_number);

    struct device *pDev;

    devNo = MKDEV(major_number, 0);  // major ve minor numaraların 32 bit kombinasyonlu hali

    // /dev/emre olusturmak icin /sys/class/emre olusturuluyor
    pClass = class_create(THIS_MODULE, device_name);

    //alloc_chrdev_region()

    if (IS_ERR(pClass)) {
        printk(KERN_WARNING "\nclass olusturulurken hata");
        unregister_chrdev_region(devNo, 1);
        return -1;
    }

    if (IS_ERR(pDev = device_create(pClass, NULL, devNo, NULL, device_name))) {
        printk(KERN_WARNING "/dev/emre olusturulurken hata\n");
        class_destroy(pClass);
        unregister_chrdev_region(devNo, 1);
        return -1;
    }

    return 0;
}

void unregister_device(void) {
    printk(KERN_NOTICE "%s unregister_device() cagrildi\n", driver_message_prefix);

    if(major_number != 0) {
        device_destroy(pClass, devNo);
        class_unregister(pClass);
        class_destroy(pClass);
        unregister_chrdev_region(devNo, 1);
        unregister_chrdev(major_number, device_name);
    }
}

static ssize_t device_file_read(struct file *file_ptr, char __user *user_buffer, size_t count, loff_t *position) {
    printk(KERN_NOTICE "%s cihaz okuma offseti = %i, okunan byte boyutu = %u\n", driver_message_prefix, (int)*position, (unsigned int) count);

    if(*position >= message_size)
        return 0;

    if(*position + count > message_size)
        count = message_size - *position; //count boyutu asarsa max olan verilecek

    if(copy_to_user(user_buffer, module_message + *position, count) != 0)
        return -EFAULT;    

    *position += count; //ileri kaydir
    return count;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Emre Harbutoglu");
MODULE_DESCRIPTION("Device driver module");

module_init(init);
module_exit(cleanup);