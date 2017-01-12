#include <unistd.h>

/* ---- Begin inclusion from src/irqm.h ---- */

#define MAX_MSG_LEN 256
#define LOG_LEN 512

/* Slightly counterintuitive. Each of driver_write, isr, and msg_dispatch
 * independently cycle through the same circular buffer of event_records.
 * 
 * */

struct event_record {
	
	/* for driver_write */
	
	int irq_user; /* irqcount at call to driver_write */
	int t_user; /* time at call to driver_write*/
	char[MAX_MSG_LEN] msg_user;
	int msg_len;
	
	/* for isr */
	
	int irq_isr; /* irqcount at interrupt */
	int t_isr;
	
	/* for msg_dispatch */
	
	int irq_dispatch; /* irqcount at call to msg_dispatch */
	int t_dispatch;
	char[MAX_MSG_LEN] msg_dispatch;
	int t_flush;
	int bytes_sent;
	
};

/* ---- End inclusion from src/irqm.h ---- */


/* Function to allow ctypes to know how many bytes of log buffer
 * there are.
 * 
 * TODO: make LOG_LEN and sizeof(event_record) available through
 * sysfs */

int nbytes(){
	
	return LOG_LEN * sizeof(event_record);
	
}


struct event_record * get(char * fname) {

	int fd;
	char * buff;

	buff = (char*) malloc(LOG_LEN * sizeof(event_record));
	
	fd = open(fname, O_WRONLY);
	read(fd, (void*) buff, LOG_LEN * sizeof(event_record));
	close(fd);
	
	return buff;
}

/* ctypes pointers are iterables */
