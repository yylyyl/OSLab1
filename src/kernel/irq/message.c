#include "thread.h"
#include "message.h"
#include "string.h"

#define MAX_MESSAGE 1024
Message message_q[MAX_MESSAGE];
int message_count = 0;
int message_head = -1;
int message_tail = -1;

extern boolean in_irq_handle;
extern PCB *current;
extern PCB pcbs[max_threads];
extern int threads;

Message* pick_empty_message() {
	int i;
	for(i=0; i<MAX_MESSAGE; i++) {
		Message *ptr = &message_q[i];
		if(!ptr->used) {
			ptr->used = TRUE;
			message_count++;
			return ptr;
		}
	}
	assert(FALSE);
	return NULL;
}

void send(pid_t dst, Message *m) {
	lock();

	m->dest = dst;
	if(in_irq_handle) {
		// msg in intr
		m->src = MSG_HWINTR;
	} else {
		m->src = current->pid;
	}
	assert(dst != ANY);
	printk("Message from %d to %d\n", m->src, dst);

	/*if(dst==ANY) {
		int i;
		for(i=1; i<threads; i++) {
			Message *ptr = pick_empty_message();
			memcpy(ptr, m, sizeof(Message));
			PCB *p = &pcbs[i];
			V(&p->msg_mutex);
		}
	} else {*/
		assert(dst < threads);
		PCB *p = &pcbs[dst];
		Message *ptr = pick_empty_message();
		memcpy(ptr, m, sizeof(Message));
		V(&p->msg_mutex);
	//}

	unlock();
}

void receive(pid_t dst, Message *m) {
	lock();
	printk("wait message to %d", dst);
	P(&current->msg_mutex);
	
	int i;
	Message *msg = NULL;
	for(i=0; i<MAX_MESSAGE; i++) {
		msg = &message_q[i];
		if(msg->used && (msg->dest==dst || dst==ANY))
			break;
	}
	assert(i != MAX_MESSAGE);
	memcpy(m, msg, sizeof(Message));

	msg->used = FALSE;

	message_count--;
	unlock();
}
