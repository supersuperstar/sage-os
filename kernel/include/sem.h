#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

#include <kernel.h>

struct semaphore {
  /* data */
};

void sem_init(sem_t *sem, const char *name, int value);
void sem_wait(sem_t *sem);
void sem_signal(sem_t *sem);

#endif