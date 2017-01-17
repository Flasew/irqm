#include "irqm.h"

MODULE_LICENSE("Dual BSD/GPL");

static int __init char_init(void) {
	
	
	params.timeout_jiffies = 1000 * HZ * params.timeout_msec;
	
	params.devno = MKDEV(params.major,0);
	cdev_init(&params.cdev, &fops);
	cdev_add(&params.cdev, params.devno,1);
	
	register_chrdev_region(params.devno,
		1, params.driver_name);

	state.workqueue = 
		create_workqueue(params.driver_name);
		
	state.f = file_open(params.target_path,
		O_WRONLY | O_CREAT,
		S_IRWXO);
		

	state.log = log_init(LOG_LEN);
		
	request_irq(params.irqno,
		(irq_handler_t) isr,
		params.irqflags,
		params.driver_name,
		(void*) &params);

	init_waitqueue_head(&state.waitqueue);

	mutex_init(&state.lock);
	state.irqcount = 0;
	
	printk(KERN_ALERT "[irqm] irq = %d, target=%s, major=%i\n",
		params.irqno,
		params.target_path,
		params.major);

	return 0;
}

static void __exit char_exit(void) {

	unregister_chrdev_region(params.devno,1);
	cdev_del(&params.cdev);
		
	flush_workqueue(state.workqueue);
	destroy_workqueue(state.workqueue);
	
	file_close(state.f);
	free_irq(params.irqno, (void *) &params);

	log_del(state.log);

	printk(KERN_ALERT "[irqm] unregistered\n");
	
	mutex_destroy(&state.lock);
}

static ssize_t driver_write(struct file *filep,
	const char *buffer,
	size_t len,
	loff_t *offset){

	ktime_t t;
	struct list_head * pos;
	struct ticket * entry;

	t = ktime_get();

	entry = NULL;

	/* Find the first overwritable ticket from state.log */
	list_for_each(pos,state.log){

		entry = list_entry(pos,struct ticket,list);
		if (entry->flags == OVERWRITABLE){

			/* claim this ticket for use here */

			WRITE_ONCE(entry->flags);

			break;
		}

	}




	WRITE_ONCE();

		
	/* block until the last msg_dispatch() returns */
	flush_workqueue(state.workqueue);	
		

	/*strncpy(state.msg,buffer,MAX_MSG_LEN);*/
	copy_from_user(state.msg,(void*)buffer,(unsigned long)len)
	state.msg_len = len;
	
	state.log[state.idx_user]->t_user = (int)t;
	state.log[state.idx_user]->irq_user = state.irqcount;
	state.log[state.idx_user]->msg_len = state.msg_len;

	strncpy(state.log[state.idx_user]->msg_user,state.msg,len);
	
	state.idx_user = ++state.idx_user % LOG_LEN;
	
	wait_event_interruptible_timeout(state.waitqueue,
		state.sent != 0,
		params.timeout_jiffies);
		
	return state.sent;
}

static int driver_release(struct inode *inodep, struct file *filep) {
	
	state.msg_len = 0;
	state.sent = 0;
	
	flush_workqueue(state.workqueue);

	mutex_unlock(&state.lock);
	
	printk(KERN_ALERT "[irqm] close\n");

	
	return 0;
	
}

static int driver_open(struct inode *inodep, struct file *filep) {

	if(!mutex_trylock(&state.lock)) {return -EBUSY;}
	
	state.msg_len = 0;
	state.sent = 0;


	printk(KERN_ALERT "[irqm] open\n");

	return 0;
	
}



irq_handler_t isr(unsigned int irq,
	void *dev_id,
	struct pt_regs *regs){
		
		
	state.log[state.idx_isr]->t_irq = (int) ktime_get();
	state.log[state.idx_isr]->irq_isr = state.irqcount;
		
	state.sent = 0; /* !!!! */
	state.irqcount++;
	
	
	state.idx_isr = ++state.log_idx % LOG_LEN;
	
	
	
	
	printk(KERN_ALERT "[irqm] isr\n");
	
		
	if (state.msg_len && state.dispatch_ready) {
		
		if (params.delay_msec) {mdelay(params.delay_msec);}
		if (params.delay_usec) {udelay(params.delay_usec);}
		
		state.dispatch_ready = 0;
		
		/* can't do a vfs write directly */
		queue_work(state.workqueue,&state.work);
		
	}
	
	
	
	wake_up_interruptible(&state.waitqueue);
	
	return (irq_handler_t) IRQ_HANDLED;
	
}

void msg_dispatch(struct work_struct * work){
	
	ktime_t t;
	
	t = ktime_get();
	
	
	/* TODO: What happens if this call hangs at all? */
	
	state.sent = file_write(state.f,0,
			state.msg,
			(unsigned int)state.msg_len);
			
	file_sync(state.f);
		
	state.log[state.idx_dispatch]->t_flush = (int) ktime_get();
	state.log[state.idx_dispatch]->t_dispatch = (int) t;
	state.log[state.idx_dispatch]->irq_dispatch = state.irqcount - 1;
	state.log[state.idx_dispatch]->msg_len = state.sent;

	strncpy(state.log[state.idx_dispatch]->msg_dispatch,state.msg,MAX_MSG_LEN);
	
	state.idx_dispatch = ++state.idx_dispatch % LOG_LEN;
	
		
	state.msg_len = 0; /* !!!!! */
	state.dispatch_ready = 1;
				
}

static ssize_t driver_read(struct file * filep, const char * buffer, size_t len, loff_t offset){
	
	/* TODO: Maybe always returning the entire buffer regardless of len and then always
	 * claiming that we copied len bytes is worth rethinking at some point */
	
	copy_to_user(buffer, state.log, sizeof(event_record)*LOG_LEN);
	
	return len; 
	
}

module_init(char_init);
module_exit(char_exit);
