#ifndef __IRQM__
#define __IRQM__

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h> 
#include <linux/mutex.h>
#include <linux/sched.h>
#include <linux/workqueue.h>
#include <linux/list.h>
#include <linux/ktime.h>
#include <linux/string.h>
#include <linux/jiffies.h>

#include <linux/wait.h>
#include <linux/delay.h>

#include <linux/interrupt.h>

#include <linux/vmalloc.h>

#include <asm/uaccess.h>

#include "vfs.h"
#include "log.h"


/* Neither of these structs are intended to be dynamically
 * initializable. They're just meant as namespaces to organize
 * an unwieldy number of globals. */

struct driver_params {
	
	char* target_path;
	char* driver_name;
	unsigned int irqno;
	int irqflags;
	
	unsigned long delay_msec;
	unsigned long delay_usec;
	
	int timeout_msec;
	unsigned long timeout_jiffies;
	
	int major;
	dev_t devno;
	struct cdev cdev;
	
};


struct driver_state {
	
	int irqcount; /* count isr calls */
		
	struct file* f; /* destination for vfs write */
	
	struct workqueue_struct* workqueue;

	struct list_head * log;

	wait_queue_head_t waitqueue;
	
	struct mutex lock; /* prevent multiple processes from using driver */
	

};

static int driver_open(struct inode *inodep, struct file *filep);
static int driver_release(struct inode *inodep, struct file *filep);

/* Copy to user a circular buffer of the previous LOG_LEN event_records */
static ssize_t driver_read(struct file * filep, const char * buffer, size_t len, loff_t offset);


/* Latch a value to send to the target on the next interrupt cycle.
 * No value is sent on the next interrupt if not called.
 * Returns the number of bytes written on the LAST cycle.
 * Blocks until previous vfs writes have completed. */

static ssize_t driver_write(struct file *filep,
	const char *buffer,
	size_t len,
	loff_t *offset);

/* Send the stored message, if any, to the target
 * device and wake the process */

irq_handler_t isr(unsigned int irq,
	void *dev_id,
	struct pt_regs *regs);

/* Worker function that the isr places in a workqueue 
 * to send message to target device via vfs_write.
 * Necessary because vfs_write can't run in an atomic
 * context.*/

void msg_dispatch(struct work_struct * work);

static struct file_operations fops = {	
	.owner = THIS_MODULE,
	.open = driver_open,
	.write = driver_write,
	.read = driver_read,
	.release = driver_release,
};


static struct driver_params params;

static struct driver_state state ;


module_param_named(irq,params.irqno, uint, 0644);
module_param_named(irqflags,params.irqflags, int, 0644);
module_param_named(major,params.major, int, 0644);
module_param_named(timeout_msec,params.timeout_msec, int, 0644);
module_param_named(delay_msec,params.delay_msec, ulong, 0644);
module_param_named(delay_usec,params.delay_usec, ulong, 0644);
module_param_named(driver_name,params.driver_name, charp, 0644);
module_param_named(target_path,params.target_path, charp, 0644);

#endif