#include <common.h>
#include <spinlock.h>

struct spinlock os_trap_lock = {"OS Trap Lock", 0, -1};

static void os_init() {
  pmm->init();
}

static void os_run() {
#ifndef TEST
  for (const char *s = "Hello World from CPU #*\n"; *s; s++) {
    putch(*s == '*' ? '0' + cpu_current() : *s);
  }

#ifdef DEBUG
  void *alloc_addr[15];
  for (int i = 0; i < 15; i++) {
    alloc_addr[i] = pmm->alloc(sizeof(int) * 1024);
  }
  for (int i = 0; i < 15; i++) {
    pmm->free(alloc_addr[i]);
  }
#endif
  while (1)
    ;
#else  // for unit test
  printf("Hello World from CPU #%d\n", cpu_current());
  halt(0);  // must halt at here, not in unit test
#endif
}

/**
 * @brief System trap entry
 *
 * @param ev
 * @param context
 * @return Context*
 */
static Context *os_trap(Event ev, Context *context) {
  return NULL;
}

/**
 * @brief register interrupt handlers.
 *
 * @param seq determines the order in which handlers are called。
 *            smaller seq are called first。
 * @param event event type, see am.h
 * @param handler
 */
static void os_on_irq(int seq, int event, handler_t handler) {
}

MODULE_DEF(os) = {
    .init   = os_init,
    .run    = os_run,
    .trap   = os_trap,
    .on_irq = os_on_irq,
};
