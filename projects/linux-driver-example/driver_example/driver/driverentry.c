#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/kobject.h>
#include <linux/device.h>
#include <asm/io.h>
#include <asm/gpio.h>
#include <linux/init.h>


MODULE_AUTHOR("Leimon");
MODULE_LICENSE("GPL");


#define  DEVICE_NODE_IDENTIFIER      "spitest"     


int  device_init(void);
void device_exit(void);
int  device_open(struct inode *i, struct file *f);
int  device_ioctl(struct inode *i, struct file *f, unsigned int function, unsigned long parameter);


struct 
{
  struct cdev             cdev;
  struct file_operations  fops;
  struct class           *class;
  struct kref             kref;
  int                     major;
  struct device          *device;
} dev;


int device_init(void)
{
  /* Register device */
  
  /* Reference: + Linux Device Drivers 3rd Edition pages 55-56 
                + uio linux kernel driver                      */
  
  /* Zero device driver structure memory */
  memset(&dev, 0, sizeof(dev));

  /* Set function pointers for certain file operations */
  dev.fops.ioctl = device_ioctl;
  dev.fops.open  = device_open;
  dev.fops.owner = THIS_MODULE;
   
  /* Register character device */
  dev.major = register_chrdev(0, DEVICE_NODE_IDENTIFIER, &dev.fops);

  kref_init(&dev.kref);

  /* Create device class ?? */
  dev.class = class_create(THIS_MODULE, DEVICE_NODE_IDENTIFIER);
  
  /* Create device ?? */
  dev.device = device_create(dev.class, 0, MKDEV(dev.major, 0), DEVICE_NODE_IDENTIFIER); 

  return 0;
}


int device_ioctl(struct inode *i, struct file *f, unsigned int function, unsigned long parameter)
{
  printk("<1>%s: ioctl (function=%d, parameter=%ld)\n", DEVICE_NODE_IDENTIFIER, function, parameter);
  
  return 0;
}


void device_exit(void) 
{ 
}


int device_open(struct inode *i, struct file *f) 
{ 
  return 0; 
}

module_init(device_init);
module_exit(device_exit);
