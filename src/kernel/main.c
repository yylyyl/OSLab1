#include "kernel.h"
#include "x86.h"
#include "vm.h"
#include "irq.h"
#include "thread.h"

extern void A(void);
extern void B(void);

extern void test_setup();

void
os_init(void) {
	init_seg();
	init_debug();
	init_idt();
	init_i8259();
	init_thread();
	printk("The OS is now working!\n");	
	sti();
	//wakeup(create_kthread(A));
	//wakeup(create_kthread(B));
	test_setup();
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

#define NBUF 5
int buf[NBUF], f = 0, r = 0, g = 1, tid = 1;
Semaphore empty, full, mutex;

void
test_producer(void) {
    while (TRUE) {
        P(&empty);
        P(&mutex);
        buf[f ++] = g ++;
        f %= NBUF;
        V(&mutex);
        V(&full);
    }
}

void
test_consumer(void) {
    int id = tid ++;
    while (TRUE) {
        P(&full);
        P(&mutex);
        printk("#%d Got: %d\n", id, buf[r ++]);
        r %= NBUF;
        V(&mutex);
        V(&empty);
    }
}

void
test_setup(void) {
    new_sem(&full, 0);
    new_sem(&empty, NBUF);
    new_sem(&mutex, 1);
    wakeup(create_kthread(test_producer));
    wakeup(create_kthread(test_producer));
    wakeup(create_kthread(test_producer));
    wakeup(create_kthread(test_consumer));
    wakeup(create_kthread(test_consumer));
    wakeup(create_kthread(test_consumer));
    wakeup(create_kthread(test_consumer));
}
