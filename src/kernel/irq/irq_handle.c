#include "x86.h"
#include "kernel.h"
#include "thread.h"

extern PCB *current;

void irq_handle(TrapFrame *tf) {
	int irq = tf->irq;
	current->tf = tf;
	assert(irq >= 0);

	if (irq < 1000) {
		// exception
		cli();
		printk("Unexpected exception #%d\n", irq);
		printk(" errorcode %x\n", tf->err);
		printk(" location  %d:%x, esp %x\n", tf->cs, tf->eip, tf);
		panic("unexpected exception");
	} else if (irq >= 1000) {
		// external interrupt
		if (irq==1000) {
			// time interrupt
                        printk("time interrupt\n");
			next_thread();
	       		printk("time interrupt ends\n");
		}
	}
}
