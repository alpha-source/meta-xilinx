#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define  DEVICE_NAME "aeva_dma"
#define  CLASS_NAME  "aevadma"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sandesh J. Ghimire");
MODULE_DESCRIPTION("Aeva Inc. dma driver ");
MODULE_VERSION("0.1");

static int major_number;

static char message[256] = {0};

static short size_of_message;

static int number_opens = 0;

static struct class *aeva_dma_class = NULL;

static struct device *aeva_dma_device = NULL;

static int dma_open(struct inode *, struct file *);
static int dma_release(struct inode *, struct file *);
static ssize_t dma_read(struct file *, char *, size_t, loff_t *);
static ssize_t dma_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops =
    {
        .open = dma_open,
        .read = dma_read,
        .write = dma_write,
        .release = dma_release,
    };

static int __init aeva_dma_init(void)
{
    printk(KERN_INFO
    "AevaDMA: Initializing the AevaDMA LKM\n");


    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        printk(KERN_ALERT
        "AevaDMA failed to register a major number\n");
        return major_number;
    }
    printk(KERN_INFO
    "AevaDMA: registered correctly with major number %d\n", major_number);

    // Register the device class
    aeva_dma_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(aeva_dma_class)) {
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ALERT
        "Failed to register device class\n");
        return PTR_ERR(aeva_dma_class);
    }
    printk(KERN_INFO
    "AevaDMA: device class registered correctly\n");


    aeva_dma_device = device_create(aeva_dma_class, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
    if (IS_ERR(aeva_dma_device)) {
        class_destroy(aeva_dma_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ALERT
        "Failed to create the device\n");
        return PTR_ERR(aeva_dma_device);
    }
    printk(KERN_INFO
    "AevaDMA: device class created correctly\n");
    return 0;
}

static void __exit aeva_dma_exit(void)
{
    device_destroy(aeva_dma_class, MKDEV(major_number, 0));
    class_unregister(aeva_dma_class);
    class_destroy(aeva_dma_class);
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO
    "AevaDMA: Goodbye from the LKM!\n");
}

static int dma_open(struct inode *inodep, struct file *filep)
{
    number_opens++;
    printk(KERN_INFO
    "AevaDMA: Device has been opened %d time(s)\n", number_opens);
    return 0;
}

static ssize_t dma_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
    int error_count = 0;

    error_count = copy_to_user(buffer, message, size_of_message);

    if (error_count == 0) {
        printk(KERN_INFO
        "AevaDMA: Sent %d characters to the user\n", size_of_message);
        return (size_of_message = 0);
    }
    else {
        printk(KERN_INFO
        "AevaDMA: Failed to send %d characters to the user\n", error_count);
        return -EFAULT;
    }
}

static ssize_t dma_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
    sprintf(message, "%s(%zu letters)", buffer, len);
    size_of_message = strlen(message);
    printk(KERN_INFO
    "AevaDMA: Received %zu characters from the user\n", len);
    return len;
}

static int dma_release(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO
    "AevaDMA: Device successfully closed\n");
    return 0;
}

module_init(aeva_dma_init);

module_exit(aeva_dma_exit);