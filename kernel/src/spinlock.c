#include <spinlock.h>

void spin_init(spinlock_t *lk, const char *name) {
  lk->lock_flag = 0;
  lk->name = name;
  lk->hold_cpuid = -1;
}

void spin_lock(spinlock_t *lk) {
  //spin_pushcli();     //interrupt disable
  //Assert();

  while(atomic_xchg((intptr_t *) &lk->lock_flag, 1)) {
    ;
  }
  //warning: this func unable before gdb version year 2008
  //to force the CPU to operate the after code after the before code
  __sync_synchronize(); //memory barrier

  lk->hold_cpuid = cpu_current();

}

void spin_unlock(spinlock_t *lk) {
  //Assert();
  lk->hold_cpuid = -1;
  __sync_synchronize(); //memory barrier
  
  asm volatile ("movl $0, %0" : "+m"(lk->lock_flag) : );
  //spin_popcli();
}