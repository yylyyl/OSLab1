#include "x86.h"
#include "kernel.h"
#include "types.h"
#include "adt/list.h"

#ifndef __THREAD_H__
#define __THREAD_H__

#define KSTACK_SIZE 8192

struct PCB {
    TrapFrame *tf;
    uint8_t kstack[KSTACK_SIZE];
    int tid;
    ListHead runq;
    ListHead sleepq;
    int lock;
};
typedef struct PCB PCB;

struct Semaphore {
    int count;
    ListHead queue;
};
typedef struct Semaphore Semaphore;

PCB *create_kthread(void *entry);
void sleep(void);
void wakeup(PCB *pcb);
void lock(void);
void unlock(void);
void init_thread(void);
void next_thread();

#endif
