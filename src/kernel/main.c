#include "kernel.h"
#include "x86.h"
#include "vm.h"
#include "irq.h"
#include "thread.h"

extern void A(void);
extern void B(void);

void
os_init(void) {
	init_seg();
	init_debug();
	init_idt();
	init_i8259();
	init_thread();
	printk("The OS is now working!\n");
	wakeup(create_kthread(A));
	wakeup(create_kthread(B));
	sti();
	while (TRUE) {
		wait_intr();
	}
}

void
entry(void) {
	init_kvm();
	void(*volatile next)(void) = os_init;
	asm volatile("addl %0, %%esp" : : ""(KOFFSET));
	next();
	panic("init code should never return");
}

void A(void) {
    while(1) {
        printk("AAA\n");
        wait_intr();
    }
}

void B(void) {
    while(1) {
        printk("BBB\n");
        wait_intr();
    }
}
