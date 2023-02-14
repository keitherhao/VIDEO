#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/io.h>
#include <asm/uaccess.h>
 
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ide.h>
#include <linux/init.h>
#include <linux/module.h>

static int major = 254;
static int minor=1;
static dev_t devno;
static struct class *cls;
static struct device *test_device;
 
#define  GPX2CON    0x11000c40
#define  GPX2DAT    0x11000c44
#define  GPX1CON    0x11000c20
#define  GPX1DAT    0x11000c24
#define  GPF3CON    0x114001e0
#define  GPF3DAT    0x114001e4
 
// static int *pgpx2con  ;
// static int *pgpx2dat;
 
// static int *pgpx1con  ;
// static int *pgpx1dat;
 
// static int *pgpf3con;
// static int *pgpf3dat;
 
void fs4412_led_off(int num);
 
void fs4412_led_on(int num)
{
	switch(num)
	{
		case 1:
			// writel(readl(pgpx2dat) |(0x1<<7), pgpx2dat);
            printk("[ %s ] line = %d num = %d\n", __FUNCTION__, __LINE__, num);
			break;
		case 2:
			// writel(readl(pgpx1dat) |(0x1<<0), pgpx1dat);
            printk("[ %s ] line = %d num = %d\n", __FUNCTION__, __LINE__, num);
			break;			
		case 3:
			// writel(readl(pgpf3dat) |(0x1<<4), pgpf3dat);	
            printk("[ %s ] line = %d num = %d\n", __FUNCTION__, __LINE__, num);
			break;
		case 4:
			// writel(readl(pgpf3dat) |(0x1<<5), pgpf3dat);	
            printk("[ %s ] line = %d num = %d\n", __FUNCTION__, __LINE__, num);
			break;	
		default:
			// fs4412_led_off(1);
			// fs4412_led_off(2);
			// fs4412_led_off(3);
			// fs4412_led_off(4);
            printk("[ %s ] line = %d num = %d\n", __FUNCTION__, __LINE__, num);
			break;
			
	}
}
 
void fs4412_led_off(int num)
{
	switch(num)
	{
		case 1:
			// writel(readl(pgpx2dat) &(~(0x1<<7)), pgpx2dat);
            printk("[ %s ] line = %d num = %d\n", __FUNCTION__, __LINE__, num);
			break;
		case 2:
			// writel(readl(pgpx1dat)&(~(0x1<<0)), pgpx1dat);
            printk("[ %s ] line = %d num = %d\n", __FUNCTION__, __LINE__, num);
			break;			
		case 3:
			// writel(readl(pgpf3dat) &(~(0x1<<4)), pgpf3dat);	
            printk("[ %s ] line = %d num = %d\n", __FUNCTION__, __LINE__, num);
			break;
		case 4:
			// writel(readl(pgpf3dat) &(~(0x1<<5)), pgpf3dat);			
            printk("[ %s ] line = %d num = %d\n", __FUNCTION__, __LINE__, num);
			break;			
	}
}
 
static int led_open (struct inode *inode, struct file *filep)
{//open
	fs4412_led_off(1);
	fs4412_led_off(2);
	fs4412_led_off(3);
	fs4412_led_off(4);	
	return 0;
}
 
static int led_release(struct inode *inode, struct file *filep)
{//close
	fs4412_led_off(1);
	fs4412_led_off(2);
	fs4412_led_off(3);
	fs4412_led_off(4);	
	return 0;
}
 
static ssize_t led_read(struct file *filep, char __user *buf, size_t len, loff_t *pos)
{
	return 0;
}
 
static ssize_t led_write(struct file *filep, const char __user *buf, size_t len, loff_t *pos)
{
	int led_num;
 
	if(len !=4)
	{
		return -EINVAL;
	}
	if(copy_from_user(&led_num,buf,len))
	{
		return -EFAULT;	
	}
 
	fs4412_led_on(led_num);
	// printk("led_num =%d \n",led_num);
    printk("[ %s ] line = %d num = %d\n", __FUNCTION__, __LINE__, led_num);
 
	return 0;
}
 
static struct file_operations hello_ops=
{
	.open     = led_open,
	.release = led_release,
	.read     = led_read,
	.write    = led_write,
};
 
static void fs4412_led_init(void)
{
	// pgpx2con = ioremap(GPX2CON,4);
	// pgpx2dat = ioremap(GPX2DAT,4);
 
	// pgpx1con = ioremap(GPX1CON,4);
	// pgpx1dat =ioremap(GPX1DAT,4);
 
	// pgpf3con  = ioremap(GPF3CON,4);
	// pgpf3dat =ioremap(GPF3DAT,4);
 
	// writel((readl(pgpx2con)& ~(0xf<<28)) |(0x1<<28),pgpx2con) ;
	// writel((readl(pgpx1con)& ~(0xf<<0)) |(0x1<<0),pgpx1con) ;	
	// writel((readl(pgpf3con)& ~(0xff<<16)) |(0x11<<16),pgpf3con) ;	
}
 
static int led_init(void)
{
	int ret;	
	devno = MKDEV(major,minor);
    printk("[ %s ] line = %d\n", __FUNCTION__, __LINE__);
	ret = register_chrdev(major,"led",&hello_ops);
    printk("[ %s ] line = %d\n", __FUNCTION__, __LINE__);
	cls = class_create(THIS_MODULE, "myclass");
    printk("[ %s ] line = %d\n", __FUNCTION__, __LINE__);
	if(IS_ERR(cls))
	{
		unregister_chrdev(major,"led");
        printk("[ %s ] line = %d\n", __FUNCTION__, __LINE__);
		return -EBUSY;
	}
	test_device = device_create(cls,NULL,devno,NULL,"led");//mknod /dev/hello
    printk("[ %s ] line = %d\n", __FUNCTION__, __LINE__);
	if(IS_ERR(test_device))
	{
		class_destroy(cls);
        printk("[ %s ] line = %d\n", __FUNCTION__, __LINE__);
		unregister_chrdev(major,"led");
        printk("[ %s ] line = %d\n", __FUNCTION__, __LINE__);
		return -EBUSY;
	}	
	fs4412_led_init();
    printk("[ %s ] line = %d\n", __FUNCTION__, __LINE__);
	return 0;
}
 
void fs4412_led_unmap(void)
{
	// iounmap(pgpx2con);
	// iounmap(pgpx2dat );
 
	// iounmap(pgpx1con);
	// iounmap(pgpx1dat );
 
	// iounmap(pgpf3con );
	// iounmap(pgpf3dat );
}
 
static void led_exit(void)
{
	fs4412_led_unmap();
	device_destroy(cls,devno);
	class_destroy(cls);	
	unregister_chrdev(major,"led");
	printk("led_exit \n");
}
 
MODULE_LICENSE("GPL");
module_init(led_init);
module_exit(led_exit);