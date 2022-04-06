#ifndef __SPINLOCK_H__
#define __SPINLOCK_H__

#include <kernel.h>

struct spinlock {
  bool lock_flag;     //check the lock locked , 1 means locked
  const char *name;   //Name of lock(for debug)
  int hold_cpuid;     //The cpu holding the lock(for debug)
};

void spin_init(spinlock_t *lk, const char *name);
void spin_lock(spinlock_t *lk);
void spin_unlock(spinlock_t *lk);


#endif