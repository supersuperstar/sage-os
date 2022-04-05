#ifndef __SPINLOCK_H__
#define __SPINLOCK_H__

#include <kernel.h>

struct spinlock {
  /* data */
};

void spin_init(spinlock_t *lk, const char *name);
void spin_lock(spinlock_t *lk);
void spin_unlock(spinlock_t *lk);

#endif