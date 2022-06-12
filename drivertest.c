#include <linux/module.h>
#include "linux/moduleparam.h"
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/semaphore.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include "linux/init.h"
#include "linux/kmod.h"
#include "linux/delay.h"
#include "linux/sched.h"
#include <linux/kthread.h>
#include <linux/ioctl.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

#define COMMAND_START '1'
#define COMMAND_PAUSE '2'
#define COMMAND_STOP '3'
#define COMMAND_RESTART '4'
#define COMMAND_START_1 '5'
#define COMMAND_START_2 '6'
#define COMMAND_PAUSE_1 '7'
#define COMMAND_PAUSE_2 '8'
#define COMMAND_STOP_1 '9'
#define COMMAND_STOP_2 'a'
#define COMMAND_CONTINUE_1 'b'
#define COMMAND_CONTINUE_2 'c'

int data1 = 1,data2 = 1;

static struct task_struct *demo_kthread_1;    //定义一个task_struct结构体>指针，赋值为NULL 
static struct task_struct *demo_kthread_2;    //定义一个task_struct结构体>指针，赋值为NULL 

struct semaphore led_sem1;// 定义信号量
struct semaphore led_sem2;
struct semaphore led_time_sem1;
struct semaphore led_time_sem2;

struct timer_list key_timer1;
struct timer_list key_timer2;
int flag1 = 0, flag2 = 0;

static void key_timer_expire1(struct timer_list *t)
{
	if(flag1 == 1){
		printk("kthread1---->%d",data1++);
		mod_timer(&key_timer1,jiffies + HZ);
	}else{
		mod_timer(&key_timer1,jiffies + HZ);
	}
}

static void key_timer_expire2(struct timer_list *t)
{
	if(flag2 == 1){
       		printk("thread2---->%d",data2++);
        	mod_timer(&key_timer2,jiffies +2*HZ);
	}else{
		mod_timer(&key_timer2,jiffies +2*HZ);
	}
}


int kthread_1(void *data)   //定义一个内核线程要执行的函数
{
	printk("kthread_1 success\n");
        int ret = 0;
        timer_setup(&key_timer1,(void *)key_timer_expire1,0);
        add_timer(&key_timer1);

	while(1)
	{
		if(kthread_should_stop())
			break;
		
		ret = down_interruptible(&led_sem1);
		if(ret){
			printk("err\n");
                	return ret;
        	}
		if(data1>100)
			data1 = 1;
		up(&led_sem1);
	}
		
        return 0;
}


int kthread_2(void *data)   //定义一个内核线程要执行的函数
{
        printk("kthread_2 success\n");
        int ret = 0;
	timer_setup(&key_timer2,(void *)key_timer_expire2,0);
       
        add_timer(&key_timer2);

        while(1)
        {
                if(kthread_should_stop())
                        break;

                ret = down_interruptible(&led_sem2);
                if(ret){
                        printk("err\n");
                        return ret ;
            	}
                if(data2>100)
                        data2 = 1;

                up(&led_sem2);
                
        }

        return 0;
}



static ssize_t demo_read(struct file *filp,char __user *buf,size_t count,loff_t *f_pos)
{
	return 0;
}

static ssize_t demo_write(struct file *filp,const char __user *buf,size_t count,loff_t *f_pos)
{
	return 0;
}

static long demo_ioctl(struct file *filp,unsigned int cmd,unsigned long arg)
{
	printk(KERN_ERR "DEMO:Line %d, function %s() has been invoked!\n",__LINE__,__func__);
	
	switch(cmd){
		case COMMAND_START:
			printk(KERN_ERR "DEMO:Line %d, ioctl successfully (COMMAND_START)!\n",__LINE__);		
			flag1 = 1;
			flag2 = 1;
			while(led_sem1.count <= 0 && led_sem2.count <= 0){
				up(&led_sem1);
				up(&led_sem2);
			}
			break;
		case COMMAND_PAUSE:
			printk(KERN_ERR "DEMO:Line %d, ioctl successfully (COMMAND_PAUSE)!\n",__LINE__);
			flag1 = 0;
			flag2 = 0;
			while(led_sem1.count > 0 && led_sem2.count > 0){
				down_interruptible(&led_sem1);
				down_interruptible(&led_sem2);
			}
			break;
		case COMMAND_STOP:
			printk(KERN_ERR "DEMO:Line %d, ioctl successfully (COMMAND_STOP)!\n",__LINE__);
			data1 = 1;
		   	data2 = 1;
			flag1 = 0;
			flag2 = 0;
			while(led_sem1.count>0 && led_sem2.count>0){
				down_interruptible(&led_sem1);
                        	down_interruptible(&led_sem2);	
			}
			break;
		case COMMAND_RESTART:
                        printk(KERN_ERR "DEMO:Line %d, ioctl successfully (COMMAND_RESTART)!\n",__LINE__); 
                        data1 = 1;
                        data2 = 1;
			flag1 = 1;
			flag2 = 1;
			while(led_sem1.count<=0 && led_sem2.count<=0){
                        	up(&led_sem1);
                        	up(&led_sem2);
			}
			break;

		case COMMAND_START_1:
                        printk(KERN_ERR "DEMO:Line %d, ioctl successfully (COMMAND_START_1)!\n",__LINE__);
                        flag1 = 1;
			while(led_sem1.count <= 0){
				up(&led_sem1);
			}
			break;

		case COMMAND_START_2:
			flag2 = 1;
                        printk(KERN_ERR "DEMO:Line %d, ioctl successfully (COMMAND_START_2)!\n",__LINE__);
                        while(led_sem2.count <= 0){
				up(&led_sem2);
			}
			break;
                case COMMAND_PAUSE_1:
			flag1 = 0;
			while(led_sem1.count > 0){
				down_interruptible(&led_sem1);
			}
                        printk(KERN_ERR "DEMO:Line %d, ioctl successfully (COMMAND_PAUSE_1)!\n",__LINE__);
                        break;
                case COMMAND_PAUSE_2:
                        printk(KERN_ERR "DEMO:Line %d, ioctl successfully (COMMAND_PAUSE_2)!\n",__LINE__);
                        flag2 = 0;
			while(led_sem2.count > 0){	
				down_interruptible(&led_sem2);
			}
			break;
                case COMMAND_STOP_1:
                        printk(KERN_ERR "DEMO:Line %d, ioctl successfully (COMMAND_STOP_1)!\n",__LINE__);
                        data1 = 1;
			flag1 = 0;
			while(led_sem1.count > 0 ){
				down_interruptible(&led_sem1);
			} 
			break;
                case COMMAND_STOP_2:
                        printk(KERN_ERR "DEMO:Line %d, ioctl successfully (COMMAND_STOP_2)!\n",__LINE__);
                        data2 = 1;
			flag2 = 0;
			while(led_sem2.count > 0){
				down_interruptible(&led_sem2);
			}
			break;
                case COMMAND_CONTINUE_1:
                        printk(KERN_ERR "DEMO:Line %d, ioctl successfully (COMMAND_PAUSE_1)!\n",__LINE__);
                        flag1 = 1;
			while(led_sem1.count <= 0){	
				up(&led_sem1);
			}
			break;
                case COMMAND_CONTINUE_2:
                        printk(KERN_ERR "DEMO:Line %d, ioctl successfully (COMMAND_PAUSE_2)!\n",__LINE__);
                        flag2 = 1;
			while(led_sem2.count <= 0){
				up(&led_sem2);
			}
			break;

		default:
	       		printk(KERN_ERR "DEMO:Line %d, ioctl error(invalid command)!\n",__LINE__);
		       	return -EINVAL;
	}
	return 0;
}

static int demo_open(struct inode *inode,struct file *filp)
{
	printk("open success\n");
	return 0;
}

static int demo_release(struct inode *inode,struct file *filp)
{
	printk("release success\n");
	return 0;
}

static struct file_operations demo_fops = {
	.owner           = THIS_MODULE,
	.read            = demo_read,
	.write           = demo_write,
	.unlocked_ioctl  = demo_ioctl,
	.open            = demo_open,
	.release         = demo_release,
};

static int __init hello_init(void)
{
	int err = -1;

	register_chrdev(237,"xkp",&demo_fops);
	printk("init success\n");

	sema_init(&led_sem1,0);	
	sema_init(&led_sem2,0);

       	//创建线程
	demo_kthread_1 = kthread_run(kthread_1, NULL, "demo_kthread1");
	if (IS_ERR(demo_kthread_1))
	{
		printk("kthread_create failed\n");
		err = PTR_ERR(demo_kthread_1);
		demo_kthread_1 = NULL;
		return err;
	}
	
	demo_kthread_2 = kthread_run(kthread_2, NULL, "demo_kthread1");
        if (IS_ERR(demo_kthread_2))
        {
                printk("kthread_create failed\n");
                err = PTR_ERR(demo_kthread_2);
                demo_kthread_2 = NULL;
                return err;
        }


	
	return 0;
}

static void __exit hello_exit(void)
{
	unregister_chrdev(237,"xkp");
	up(&led_sem1);
	up(&led_sem2);
	kthread_stop(demo_kthread_1);
	demo_kthread_1 = NULL;
	kthread_stop(demo_kthread_2);
	demo_kthread_2 = NULL;
	flag1 = 0;
	flag2 = 0; 
	del_timer(&key_timer1);
	del_timer(&key_timer2);
	printk("exit success\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
