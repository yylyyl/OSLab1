#include "kernel.h"
#include "x86.h"
#include "vm.h"
#include "irq.h"
#include "thread.h"
#include "hal.h"
#include "term.h"
#include "time.h"
#include "tty.h"
#include "message.h"

extern void A(void);
extern void B(void);

extern void test_setup();
extern void test();

void test_msg();
void ttyd(void);

void
os_init(void) {
	init_seg();
	init_debug();
	init_idt();
	init_i8259();
	init_thread();
	printk("The OS is now working!\n");
	//printk("%d %d\n", sizeof(Message), sizeof(DevMessage));
	//init_hal();
	//init_timer();
	//init_tty();

	//TTY = 1;
	//wakeup(create_kthread(ttyd));

	//test();
	test_msg();
	sti();
	//wakeup(create_kthread(A));
	//wakeup(create_kthread(B));
	//test_setup();
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
    int i;
    for(i=0;i<999;i++)
        printk(".");
    Message m;
    send(2, &m);
}

void B(void) {
    Message m;
    receive(ANY, &m);
    printk("ok\n");
}

void
test_msg(void) {
    wakeup(create_kthread(A));
    wakeup(create_kthread(B));
}
