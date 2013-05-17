#include "x86.h"
#include "kernel.h"
#include "types.h"
#include "adt/list.h"

#ifndef __THREAD_H__
#define __THREAD_H__

#define KSTACK_SIZE 8192
#define max_threads 64

struct Semaphore {
    int count;
    ListHead queue;
};
typedef struct Semaphore Semaphore;

struct PCB {
    TrapFrame *tf;
    uint8_t kstack[KSTACK_SIZE];
    pid_t pid;
    ListHead runq;
    ListHead sleepq;
    ListHead semq;
    int lock;
    Semaphore msg_mutex;
};
typedef struct PCB PCB;

PCB *create_kthread(void *entry);
void sleep(void);
void wakeup(PCB *pcb);
void lock(void);
void unlock(void);
void init_thread(void);
void next_thread();

void new_sem(Semaphore *sem, int value);
void P(Semaphore *sem);
void V(Semaphore *sem);

#endif
