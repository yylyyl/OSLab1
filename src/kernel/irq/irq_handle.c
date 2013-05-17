#include "x86.h"
#include "kernel.h"
#include "thread.h"

extern PCB *current;
boolean in_irq_handle;

#define max_irq 32
struct irq_handler {
	int irq;
	void (*func)(void);
};
typedef struct irq_handler irq_handler;

irq_handler handler[max_irq];
int handler_count = 0;

void irq_handle(TrapFrame *tf) {
	int irq = tf->irq;
	in_irq_handle = TRUE;

	current->tf = tf;
	assert(irq >= 0);

	if (irq < 1000) {
		// exception
		if(irq == 0x80) {
			next_thread();
//printk("a");
			in_irq_handle = FALSE;
			return;
		}
		cli();
		printk("Unexpected exception #%d\n", irq);
		printk(" errorcode %x\n", tf->err);
		printk(" location  %d:%x, esp %x\n", tf->cs, tf->eip, tf);
		panic("unexpected exception");
	} else if (irq >= 1000) {
		// external interrupt
		if (irq==1000) {
			// time interrupt
                        //printk("time interrupt\n");
			next_thread();
	       		//printk("time interrupt ends\n");
		}
		int i;
		for(i=0; i<handler_count; i++) {
			irq_handler *ptr = &handler[i];
			if(ptr->irq == irq - 1000) {
				ptr->func();
			}
		}
	}
	in_irq_handle = FALSE;
}

void add_irq_handle(int irq, void (*func)(void)) {
	assert(handler_count < max_irq);
	irq_handler *ptr = &handler[handler_count++];
	ptr->irq = irq;
	ptr->func = func;
}
