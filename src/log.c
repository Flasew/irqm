#include "log.h"

struct list_head * log_init(int len){

	int i;
	struct ticket * entry;
	struct list_head * pos;

	entry = (struct ticket*) kzalloc(sizeof(struct ticket),GFP_KERNEL);
	INIT_LIST_HEAD(entry->list);

	pos = &(entry->list);

	for(i=0; i < (len-1); i++){

		entry = (struct ticket*) kzalloc(sizeof(struct ticket),GFP_KERNEL);
		INIT_LIST_HEAD(entry->list);

		entry->flags = OVERWRITABLE;

		list_add(entry->list, pos);

		pos = &(entry->list);

	}

	return pos;
}

void log_del(struct list_head * list){

	struct list_head * next, pos;

	list_for_each_safe(pos,next,list){

		entry = list_entry(pos, struct ticket, list);
		kfree(entry);

	}

}

struct list_head * log_find_oldest(struct list_head * head){

	int t;
	struct list_head * pos;
	struct ticket * entry;

	t = list_entry(head,struct ticket,list)->t_creation;


	list_for_each(pos,state.log){

		entry = list_entry(pos,struct ticket,list);
		if (entry->t_creation < t){

			

		}

	}


}

struct list_head * log_find_newest(struct list_head * head){

	
	
}