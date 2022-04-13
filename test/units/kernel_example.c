#include <kernel.h>
#include <klib.h>
#include <logger.h>
#include <spinlock.h>

spinlock_t lock;

static void run() {
  spin_lock(&lock);
  info("Hello World from CPU #%d\n", cpu_current());
  spin_unlock(&lock);
  while (1)
    ;
}

int main() {
  ioe_init();
  spin_init(&lock, "lock");
  mpe_init(&run);
}