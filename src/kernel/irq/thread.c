#include "thread.h"
#include "adt/list.h"
#include "types.h"
#include "string.h"

PCB *current = NULL;

#define max_threads 32
int threads = 1;
PCB pcbs[max_threads];
ListHead runq_h;
ListHead sleepq_h;

void init_thread(void) {
    memset(&pcbs, 0, max_threads*sizeof(PCB));
    list_init(&runq_h);
    list_init(&sleepq_h);
    current = &pcbs[0];
}

PCB *create_kthread(void *entry) {
    PCB *pcb = &pcbs[threads];
    TrapFrame *tf = ((TrapFrame *)(pcb->kstack + KSTACK_SIZE)) - 1;
    memset(tf, 0, sizeof(TrapFrame));

    pcb->tf = tf;
    tf->eip = (uint32_t)entry;
    tf->ds = KSEL(SEG_KDATA);
    tf->es = KSEL(SEG_KDATA);
    tf->cs = KSEL(SEG_KCODE);
    tf->eflags = 0x00000200;
    pcb->tid = threads;
    printk("Created thread %d\n", threads);
    list_add_before(&sleepq_h, &pcb->sleepq);
    threads++;

    return pcb;
}


void sleep(void) {
    list_del(&current->runq);
    list_add_before(&sleepq_h, &current->sleepq);
    asm volatile("int $0x80");
}

void wakeup(PCB *pcb) {
    printk("wake up thread %d\n", pcb->tid);
    list_add_before(&runq_h, &pcb->runq);
    list_del(&pcb->sleepq);
}

void lock(void) {
    cli();
    current->lock++;
}

void unlock(void) {
    current->lock--;
    assert(current->lock >= 0);
    if(current->lock == 0) {
        sti();
    }
}

void next_thread() {
    if(list_empty(&runq_h)) {
        printk("running queue is empty\n");
        return;
    }
    if(current==list_entry(runq_h.prev, PCB, runq) || current->runq.next==NULL) {
        // current->runq.next==NULL ? It is kernel's first thread which is not in runq.
        current = list_entry(runq_h.next, PCB, runq);
    } else {
        current = list_entry(current->runq.next, PCB, runq);
    }

    printk("Thread %d is going to run\n", current->tid);
}
