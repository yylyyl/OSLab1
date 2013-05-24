#include "thread.h"
#include "adt/list.h"
#include "types.h"
#include "string.h"

PCB *current;
PCB *temp;

#define max_threads 32
int threads = 1;
PCB pcbs[max_threads];
ListHead runq_h;
ListHead sleepq_h;
ListHead semq_h;

void init_thread(void) {
    memset(&pcbs, 0, max_threads*sizeof(PCB));
    list_init(&runq_h);
    list_init(&sleepq_h);
    list_init(&semq_h);

    current = &pcbs[0]; // let the kernel first thread to have a pcb
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
    tf->eflags = FL_IF;
    pcb->tid = threads;
    printk("Created thread %d\n", threads);
    list_add_before(&sleepq_h, &pcb->sleepq);
    threads++;

    return pcb;
}


void sleep(void) {
    temp = list_entry(current->runq.next, PCB, runq);
    list_del(&current->runq);
    list_add_before(&sleepq_h, &current->sleepq);
    asm volatile("int $0x80");
}

void wakeup(PCB *pcb) {
    //printk("wake up thread %d\n", pcb->tid);
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
        //printk("running queue is empty\n");
        return;
    }
    if(current==list_entry(runq_h.prev, PCB, runq) || current->runq.next==current->runq.prev) {
        // current->runq.next==NULL ? It is kernel's first thread which is not in runq.
        current = list_entry(runq_h.next, PCB, runq);
    } else {
        current = list_entry(current->runq.next, PCB, runq);
    }

    //printk("Thread %d is going to run\n", current->tid);
}

void new_sem(Semaphore *sem, int value) {
    sem->count = value;
    list_init(&sem->queue);
}

void P(Semaphore *sem) {
    sem->count --;
    if (sem->count < 0) {
        list_add_before(&sem->queue, &current->semq);
        sleep(); // 令当前进程立即进入睡眠
    }
}

void V(Semaphore *sem) {
    sem->count ++;
    if (sem->count <= 0) {
        assert(!list_empty(&sem->queue));
        PCB *pcb = list_entry(sem->queue.next, PCB, semq);
        list_del(sem->queue.next);
        wakeup(pcb); // 唤醒PCB所对应的进程
    }
}
