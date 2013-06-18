#include "thread.h"
#include "message.h"
#include "string.h"

#define MAX_MESSAGE 1024
Message message_q[MAX_MESSAGE];
boolean message_used[MAX_MESSAGE];
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
		if(!message_used[i]) {
			message_count++;
			//printk("msg %d used\n", i);
			message_used[i] = TRUE;
			return &message_q[i];
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
	//printk("Message from %d to %d\n", m->src, dst);

	/*if(dst==ANY) {
		int i;
		for(i=1; i<threads; i++) {
			Message *ptr = pick_empty_message();
			memcpy(ptr, m, sizeof(Message));
			PCB *p = &pcbs[i];
			V(&p->msg_mutex);
		}
	} else {*/
		//assert(dst < threads);
		PCB *p = &pcbs[dst];
		assert(p->pid == dst);
		Message *ptr = pick_empty_message();
		memcpy(ptr, m, sizeof(Message));
		//ptr->used = TRUE;
		V(&p->msg_mutex);
	//}

	unlock();
}

void receive(pid_t src, Message *m) {
	lock();
	//printk("Thread %d waits message from %d\n", current->pid, src);
	P(&current->msg_mutex);
	
	//printk("Thread %d received message from %d\n", current->pid, src);
	int i;
	Message *msg = NULL;
	for(i=0; i<MAX_MESSAGE; i++) {
		msg = &message_q[i];
		if(message_used[i] && msg->dest==current->pid && (msg->src==src || src==ANY))
			break;
	}
	assert(i != MAX_MESSAGE);
	memcpy(m, msg, sizeof(Message));
	//printk("msg %d freed\n", i);
	message_used[i] = FALSE;

	message_count--;
	unlock();
}
