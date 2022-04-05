#ifndef __SPINLOCK_H__
#define __SPINLOCK_H__

#include <kernel.h>

struct spinlock_t {
  bool lock_flag;     //check the lock locked , 1 means locked
  const char *name;   //Name of lock
  int hold_cpuid;     //The cpu holding the lock
  int pcs[10];        //The call stack that locked the lock(for debugger)
};

void spin_init(spinlock_t *lk, const char *name);
void spin_lock(spinlock_t *lk);
void spin_unlock(spinlock_t *lk);


#endif