
/* Every call to driver_write creates a new ticket containing
   a timestamp, current irq count, and the user message buffer.
   On each interrupt, we check for a ticket that matches the current
   interrupt count. If found, the isr stamps the ticket with the 
   current time and ISR_SERVICED, then schedules a worker thread
   to dispatch the ticket's message buffer to the target device.

   If that ticket is in use by driver_write, it stamps the ticket
   with ISR_NOMSG and returns without spinning off a worker.

   Finally, msg_dispatch, if called, stamps the ticket with the time,
   current irq count, and bytes successfully sent.

   Tickets are stored in a circular linked list.

 */

#ifndef __IRQM__LOG__
#define __IRQM__LOG__

#include <linux/list.h>
#include <linux/slab.h>

#define MAX_MSG_LEN 128
#define LOG_LEN 64

#define WRITE_INUSE (1) /* driver_write is currently using ticket */
#define DISPATCH_INUSE (1<<1) /* msg_dispatch is currently using ticket */
#define ISR_INUSE (1<<2) /* isr is currently using ticket */
#define ISR_NOMSG (1<<3)
#define ISR_SERVICED (1<<4)
#define USER_READABLE (1<<5) /* no more action will be taken, and can be read to user */
#define DISPATCH_LATE (1<<6) /* irq count at msg_dispatch is higher than at creation */
#define OVERWRITABLE (1<<7) /* blank or overwritable. initially set. */


struct ticket {
	
	struct list_head list;

	struct work_struct work;

	int flags;

	int irqno_creation; /* internal irq count at ticket creation */
	int t_creation;

	int t_user;
	char msg[MAX_MSG_LEN];
	int len;

	int t_isr;
	int irqno_isr;

	int t_dispatch; /* time at msg_dispatch call */
	int t_flush; /* time at msg_dispatch return */
	int irqno_dispatch;
	int bytes_sent;
};

void log_del(struct list_head * list);
struct list_head * log_init(int len);
struct list_head * log_find_oldest(struct list_head * head);
struct list_head * log_find_newest(struct list_head * head);
#endif