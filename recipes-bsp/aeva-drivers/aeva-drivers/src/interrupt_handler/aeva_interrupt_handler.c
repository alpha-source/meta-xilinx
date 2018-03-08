#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/interrupt.h>

#define AEVA_GPIO1_IRQ_NUMBER 120
#define AEVA_GPIO2_IRQ_NUMBER 121
#define AEVA_PC_IRQ_NUMBER 123
#define AEVA_IFM_IRQ_NUMBER 124


#define DEVICE_NAME "aeva_interrupt_handler"
#define CLASS_NAME "aevainterrupthandler"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sandesh J. Ghimire");
MODULE_DESCRIPTION("Aeva Inc. interrupt_handler driver ");
MODULE_VERSION("0.1");

static int major_number;

static char message[256] = {0};

static short size_of_message;

static int number_opens = 0;

static struct class *aeva_interrupt_handler_class = NULL;
static struct device *aeva_interrupt_handler_device = NULL;

static struct timer_list aeva_10ms_perodic_timer;
static unsigned int aeva_interrupt_flag = 0;

static int interrupt_handler_open(struct inode *, struct file *);
static int interrupt_handler_release(struct inode *, struct file *);
static ssize_t interrupt_handler_read(struct file *, char *, size_t, loff_t *);
static ssize_t interrupt_handler_write(struct file *, const char *, size_t, loff_t *);
void aeva_10ms_perodic_timer_handler(unsigned long data);
static irqreturn_t aeva_interrupt_handler_isr(int irq, void *dev_id);

static struct file_operations fops =
    {
        .open = interrupt_handler_open,
        .read = interrupt_handler_read,
        .write = interrupt_handler_write,
        .release = interrupt_handler_release,
};

static int __init aeva_interrupt_handler_init(void)
{
    int rc = 0;
    printk(KERN_INFO "AevaInterruptHandler: Initializing the AevaInterruptHandler LKM\n");

    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0)
    {
        printk(KERN_ALERT
               "AevaInterruptHandler failed to register a major number\n");
        return major_number;
    }
    printk(KERN_INFO
           "AevaInterruptHandler: registered correctly with major number %d\n",
           major_number);

    // Register the device class
    aeva_interrupt_handler_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(aeva_interrupt_handler_class))
    {
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ALERT
               "Failed to register device class\n");
        return PTR_ERR(aeva_interrupt_handler_class);
    }
    printk(KERN_INFO
           "AevaInterruptHandler: device class registered correctly\n");

    aeva_interrupt_handler_device = device_create(aeva_interrupt_handler_class, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
    if (IS_ERR(aeva_interrupt_handler_device))
    {
        class_destroy(aeva_interrupt_handler_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create the device\n");
        return PTR_ERR(aeva_interrupt_handler_device);
    }
    printk(KERN_INFO "AevaInterruptHandler: device class created correctly\n");

    setup_timer(&aeva_10ms_perodic_timer, aeva_10ms_perodic_timer_handler, 0);
    mod_timer(&aeva_10ms_perodic_timer, jiffies + msecs_to_jiffies(100));

    rc = request_irq(AEVA_GPIO1_IRQ_NUMBER, aeva_interrupt_handler_isr, IRQF_TRIGGER_MASK, CLASS_NAME, NULL);
    if (rc < 0)
    {
        printk(KERN_ALERT "%s: request_irg failed with %d\n", __func__, rc);
    }

    rc = request_irq(AEVA_GPIO2_IRQ_NUMBER, aeva_interrupt_handler_isr, IRQF_TRIGGER_MASK, CLASS_NAME, NULL);
    if (rc < 0)
    {
        printk(KERN_ALERT "%s: request_irg failed with %d\n", __func__, rc);
    }

    rc = request_irq(AEVA_PC_IRQ_NUMBER, aeva_interrupt_handler_isr, IRQF_TRIGGER_MASK, CLASS_NAME, NULL);
    if (rc < 0)
    {
        printk(KERN_ALERT "%s: request_irg failed with %d\n", __func__, rc);
    }

    rc = request_irq(AEVA_IFM_IRQ_NUMBER, aeva_interrupt_handler_isr, IRQF_TRIGGER_MASK, CLASS_NAME, NULL);
    if (rc < 0)
    {
        printk(KERN_ALERT "%s: request_irg failed with %d\n", __func__, rc);
    }

    return 0;
}

static irqreturn_t aeva_interrupt_handler_isr(int irq, void *dev_id)
{
    if (aeva_interrupt_flag == 0)
    {
        aeva_interrupt_flag = 1;
    }
    return IRQ_RETVAL(1);
}

void aeva_10ms_perodic_timer_handler(unsigned long data)
{
    mod_timer(&aeva_10ms_perodic_timer, jiffies + msecs_to_jiffies(100));
    if (aeva_interrupt_flag)
    {
        printk(KERN_INFO "AevaInterruptHandler: Bingo Interrupt got triggred !\n");
        aeva_interrupt_flag = 0;
    }
    else
    {
        printk(KERN_INFO "AevaInterruptHandler: waiting for Interrupt !\n");
    }
}

static void __exit aeva_interrupt_handler_exit(void)
{
    del_timer(&aeva_10ms_perodic_timer);
    device_destroy(aeva_interrupt_handler_class, MKDEV(major_number, 0));
    class_unregister(aeva_interrupt_handler_class);
    class_destroy(aeva_interrupt_handler_class);
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "AevaInterruptHandler: Goodbye from the LKM!\n");
}

static int interrupt_handler_open(struct inode *inodep, struct file *filep)
{
    number_opens++;
    printk(KERN_INFO
           "AevaInterruptHandler: Device has been opened %d time(s)\n",
           number_opens);
    return 0;
}

static ssize_t interrupt_handler_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
    int error_count = 0;

    error_count = copy_to_user(buffer, message, size_of_message);

    if (error_count == 0)
    {
        printk(KERN_INFO "AevaInterruptHandler: Sent %d characters to the user\n", size_of_message);
        return (size_of_message = 0);
    }
    else
    {
        printk(KERN_INFO "AevaInterruptHandler: Failed to send %d characters to the user\n", error_count);
        return -EFAULT;
    }
}

static ssize_t interrupt_handler_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
    sprintf(message, "%s(%zu letters)", buffer, len);
    size_of_message = strlen(message);
    printk(KERN_INFO
           "AevaInterruptHandler: Received %zu characters from the user\n",
           len);
    return len;
}

static int interrupt_handler_release(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO
           "AevaInterruptHandler: Device successfully closed\n");
    return 0;
}

module_init(aeva_interrupt_handler_init);

module_exit(aeva_interrupt_handler_exit);