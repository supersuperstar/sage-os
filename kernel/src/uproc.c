#include <kernel.h>
#include "syscall_defs.h"
#include <thread.h>

#include "initcode.inc"

// extern int sys_kputc(char ch);

void uproc_pgmap(task_t *proc, void *vaddr, void *paddr, int prot);
Context *uproc_syscall(Event ev, Context *context);
int uproc_create(task_t *proc, const char *name);
void uproc_init();
Context *uproc_pagefault(Event ev, Context *context);

/**
 * @brief map one physical page to one virtual page.
 *
 * @param proc PCB
 * @param vaddr virtual page addr
 * @param paddr physical page addr
 * @param prot protection bit
 */
void uproc_pgmap(task_t *proc, void *vaddr, void *paddr, int prot) {
  // TODO: need to record mapped pages for proc?
  uintptr_t va = (uintptr_t)vaddr;
  info("map va:0x%06x%06x -> pa:0x%x", va >> 24, va & ((1L << 24) - 1), paddr);
  // function map already has checks
  map(&proc->as, vaddr, paddr, prot);
}

/**
 * @brief create a user process.
 *        code will not be copied here.
 *
 * @param proc PCB
 * @param name process name
 */
int uproc_create(task_t *proc, const char *name) {
  assert_msg(proc != NULL && name != NULL, "null arguments in uproc_create");
  proc->pid      = kmt_next_pid();
  proc->name     = name;
  proc->entry    = NULL;
  proc->arg      = NULL;
  proc->state    = ST_E;
  proc->owner    = -1;
  proc->count    = 0;
  proc->wait_sem = NULL;
  proc->killed   = 0;
  proc->next     = NULL;

  memset(proc->fenceA, FILL_FENCE, sizeof(proc->fenceA));
  memset(proc->stack, FILL_STACK, sizeof(proc->stack));
  memset(proc->fenceB, FILL_FENCE, sizeof(proc->fenceB));

  Area kstack = {(void *)proc->stack,
                 (void *)proc->stack + sizeof(proc->stack)};
  protect(&proc->as);
  // entry: start addr of proc
  AddrSpace *as = &proc->as;
  proc->context = ucontext(as, kstack, as->area.start);

  // copy init code
  void *paddr = pmm->pgalloc();
  // ev.ref: actual failed virtual addr
  // vaddr is the start addr of that page
  void *vaddr = as->area.start;
  memcpy(paddr, _init, _init_len);
  uproc_pgmap(proc, vaddr, paddr, MMAP_READ | MMAP_WRITE);

  assert_msg(!spin_holding(&task_list_lock), "already hold task_list_lock");
  kmt->spin_lock(&task_list_lock);
  task_t *tp = &root_task;
  while (tp->next)
    tp = tp->next;
  tp->next = proc;
  kmt->spin_unlock(&task_list_lock);

  return proc->pid;
}

void uproc_init() {
  task_t *initproc = pmm->alloc(sizeof(task_t));
  uproc_create(initproc, "initcode");
  os->on_irq(0, EVENT_SYSCALL, uproc_syscall);
  os->on_irq(0, EVENT_PAGEFAULT, uproc_pagefault);
}

Context *uproc_pagefault(Event ev, Context *context) {
  uintptr_t ref = (uintptr_t)ev.ref;  // 48bit unsigned long
  info("pagefault: %06x%06x", ref >> 24, (uint32_t)ref & ((1L << 24) - 1));
  AddrSpace *as = &cpu_tasks[cpu_current()]->as;
  void *paddr   = pmm->pgalloc();
  // ev.ref: actual failed virtual addr
  // vaddr is the start addr of that page
  uintptr_t vaddr = ref & ~(as->pgsize - 1L);
  uproc_pgmap(cpu_tasks[cpu_current()], (void *)vaddr, paddr,
              MMAP_READ | MMAP_WRITE);
  return NULL;
}

Context *uproc_syscall(Event ev, Context *context) {
  switch (context->rax) {
    case SYS_kputc:
      putch(context->rdi);
      break;
    default:
      break;
  }
  return NULL;
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
    .init = uproc_init,
    // .kputc = uproc_kputc,
};
