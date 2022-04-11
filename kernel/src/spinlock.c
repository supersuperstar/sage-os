#include <spinlock.h>

int efif[MAX_CPU] = {};
int ncli[MAX_CPU] = {};
/**
 * @brief create spinlock
 *
 * @param lk address of spinlock example
 * @param name name of spinlock
 */
void spin_init(spinlock_t *lk, const char *name) {
  lk->lock_flag  = false;
  lk->name       = name;
  lk->hold_cpuid = cpu_current();
}

/**
 * @brief acquire spinlock to current cpu
 *
 * @param lk address of spinlock example
 */
void spin_lock(spinlock_t *lk) {
  spin_pushcli();  // interrupt disable
  // assert(!spin_holding(lk), "Acquiring lock %s when holding it.", lk->name);
  //  assert there is lock(a) and lock(a) in code
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
  // assert(spin_holding(lk), "Releasing lock %s not holded by cpu %d.",
  // lk->name,cpu_current());
  //  assert no lock but unlock
  lk->hold_cpuid = -1;
  __sync_synchronize();  // memory barrier

  // Release the lock, equivalent to lk->locked = 0.
  // This code can't use a C assignment, since it might
  // not be atomic. A real OS would use C atomics here.
  asm volatile("movl $0, %0" : "+m"(lk->lock_flag) :);
  spin_popcli();  // interrupt able
}

bool spin_holding(spinlock_t *lk) {
  bool res = 0;
  spin_pushcli();
  res = lk->lock_flag && lk->hold_cpuid == cpu_current();
  spin_popcli();
  return res;
}

void spin_pushcli() {
  iset(false);

  if (ncli[cpu_current()] == 0) {
    efif[cpu_current()] = ienabled();
  }
  ncli[cpu_current()] += 1;
}

void spin_popcli() {
  ncli[cpu_current()] -= 1;
  // assert(ncli[cpu_current()] >= 0, "Cli level is negative.");

  if (ncli[cpu_current()] == 0 && efif[cpu_current()]) {
    iset(true);
  }
}
