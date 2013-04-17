#include "kernel.h"
#include "x86.h"
#include "vm.h"
#include "irq.h"

void
os_init(void) {
	init_seg();
	init_debug();
	init_idt();
	init_i8259();
	printk("The OS is now working!\n");
	printk("vfprintf test starts!\n");
	printk("test %%d: %d %d(negtive)\n", 9876654, -981234);
	printk("test %%x: %x %x\n", 9876654, 981234);
	printk("test %%s: %s %s(empty)\n", "hello~~~", "");
	printk("test %%c: %c %c %c\n", 'H', 'c', '9');
	printk("vfprintf test ends!\n");
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

