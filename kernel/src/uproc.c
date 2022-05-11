#include <kernel.h>
#include <syscall.h>

#include "initcode.inc"

void uproc_init() {
  //   vme_init(pmm->alloc, pmm->free);
  //   task_t *initproc = pmm->alloc(sizeof(task_t));
  //   kmt->create(initproc, "initproc", )
}

int uproc_kputc(task_t *task, char ch) {
  putch(ch);
  return 0;
}

// int uproc_fork(task_t *task);
// int uproc_wait(task_t *task, int *status);
// int uproc_exit(task_t *task, int status);
// int uproc_kill(task_t *task, int pid);
// void *uproc_mmap(task_t *task, void *addr, int length, int prot, int flags);
// int uproc_getpid(task_t *task);
// int uproc_sleep(task_t *task, int seconds);
// int64_t uproc_uptime(task_t *task);

MODULE_DEF(uproc) = {
    .init  = uproc_init,
    .kputc = uproc_kputc,
};
