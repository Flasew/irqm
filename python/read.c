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
	char msg_user[MAX_MSG_LEN];
	int msg_len;
	
	/* for isr */
	
	int irq_isr; /* irqcount at interrupt */
	int t_isr;
	
	/* for msg_dispatch */
	
	int irq_dispatch; /* irqcount at call to msg_dispatch */
	int t_dispatch;
	char msg_dispatch[MAX_MSG_LEN];
	int t_flush;
	int bytes_sent;
	
};



/* ---- End inclusion from src/irqm.h ---- */

typedef char MSG[MAX_MSG_LEN];

struct formatted_log {

	/* for driver_write */
	
	int irq_user[LOG_LEN]; /* irqcount at call to driver_write */
	int t_user[LOG_LEN]; /* time at call to driver_write*/
	MSG msg_user[LOG_LEN];
	int msg_len[LOG_LEN];
	
	/* for isr */
	
	int irq_isr[LOG_LEN]; /* irqcount at interrupt */
	int t_isr[LOG_LEN];
	
	/* for msg_dispatch */
	
	int irq_dispatch[LOG_LEN]; /* irqcount at call to msg_dispatch */
	int t_dispatch[LOG_LEN];
	MSG msg_dispatch[LOG_LEN];
	int t_flush[LOG_LEN];
	int bytes_sent[LOG_LEN];

};

/* Function to allow ctypes to know how many bytes of log buffer
 * there are.
 * 
 * TODO: make LOG_LEN and sizeof(event_record) available through
 * sysfs */

int log_len(){

	return LOG_LEN;
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

void cfree(void * ptr) {

	free(ptr);
}

/* ctypes pointers are iterables */
