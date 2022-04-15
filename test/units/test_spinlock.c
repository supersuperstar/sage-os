#include "trap.h"
#include "spinlock.h"

extern int efif[MAX_CPU];
extern int ncli[MAX_CPU];
int main() {
  spinlock_t testLock;
  spin_init(&testLock, "test");
  check(testLock.lock_flag == false);
  check(strcmp(testLock.name, "test") == 0);
  check(testLock.hold_cpuid == -1);
  spinlock_t lockTest;
  spin_init(&lockTest, "test2");
  check(strcmp(lockTest.name, "test2") == 0);
  bool flag = ienabled();
  spin_lock(&testLock);
  check(testLock.lock_flag == 1);
  check(testLock.hold_cpuid == 0);
  check(spin_holding(&testLock));
  check(efif[0] == flag);
  check(ncli[0] == 1);
  spin_lock(&lockTest);
  check(efif[0] == flag);
  check(ncli[0] == 2);
  spin_unlock(&testLock);
  check(efif[0] == flag);
  check(ncli[0] == 1);
  check(!spin_holding(&testLock));
  check(testLock.lock_flag == 0);
  check(testLock.hold_cpuid == -1);
  spin_unlock(&lockTest);
  check(ncli[0] == 0);
  bool jud = ienabled();
  check(flag == jud);
  check(lockTest.lock_flag == 0);
  check(lockTest.hold_cpuid == -1);

  return 0;
}
