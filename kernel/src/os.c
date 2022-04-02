#include <common.h>

static void os_init() {
  pmm->init();
}

static void os_run() {
  for (const char* s = "Hello World from CPU #*\n"; *s; s++) {
    putch(*s == '*' ? '0' + cpu_current() : *s);
  }

#ifdef DEBUG
  void* alloc_addr[15];
  for (int i = 0; i < 15; i++) {
    alloc_addr[i] = pmm->alloc(sizeof(int) * 1024);
  }
  for (int i = 0; i < 15; i++) {
    pmm->free(alloc_addr[i]);
  }
#endif
  while (1)
    ;
}

MODULE_DEF(os) = {
    .init = os_init,
    .run  = os_run,
};
