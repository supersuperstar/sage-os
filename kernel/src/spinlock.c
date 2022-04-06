#include <spinlock.h>
/**
 * @brief create spinlock
 *
 * @param lk address of spinlock example
 * @param name name of spinlock
 */
void spin_init(spinlock_t *lk, const char *name) {
  lk->lock_flag  = false;
  lk->name       = name;
  lk->hold_cpuid = -1;
}

/**
 * @brief acquire spinlock to current cpu
 *
 * @param lk address of spinlock example
 */
void spin_lock(spinlock_t *lk) {
  // spin_pushcli();     //interrupt disable
  // Assert();

  while (atomic_xchg((int *)&lk->lock_flag, 1)) {
    ;
  }
  // warning: this func unable before gdb version year 2008
  // to force the CPU to operate the after code after the before code
  __sync_synchronize();  // memory barrier

  lk->hold_cpuid = cpu_current();
}

/**
 * @brief release spinlock from current cpu
 *
 * @param lk address of spinlock example
 */
void spin_unlock(spinlock_t *lk) {
  // Assert();
  lk->hold_cpuid = -1;
  __sync_synchronize();  // memory barrier

  // Release the lock, equivalent to lk->locked = 0.
  // This code can't use a C assignment, since it might
  // not be atomic. A real OS would use C atomics here.
  asm volatile("movl $0, %0" : "+m"(lk->lock_flag) :);
  // spin_popcli();  //interrupt able
}