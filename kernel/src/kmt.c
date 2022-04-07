#include <common.h>
#include <sem.h>
#include <spinlock.h>
#include <thread.h>

static uint32_t next_pid        = 1;
const char *task_states_trans[] = {"Unused",   "Embryo",   "To sleep",
                                   "Sleeping", "Waken up", "Running",
                                   "Zombie",   "Special"};

extern spinlock_t os_trap_lock;
task_t root_task;

Context *null_contexts[MAX_CPU] = {};
task_t *cpu_tasks[MAX_CPU]      = {};
/**
 * @brief initialize kmt module
 *
 */
static void kmt_init() {
  root_task.pid   = next_pid++;
  root_task.name  = "Root Task";
  root_task.state = ST_X;
  memset(root_task.stack, FILL_STACK, sizeof(root_task.stack));
  INIT_LIST_HEAD(&root_task.list);

  os->on_irq(INT32_MIN, EVENT_NULL, kmt_context_save);
  os->on_irq(0, EVENT_ERROR, kmt_error);
  os->on_irq(0, EVENT_IRQ_TIMER, kmt_timer);
  os->on_irq(0, EVENT_YIELD, kmt_yield);
  os->on_irq(INT32_MAX, EVENT_NULL, kmt_context_switch);
}

/**
 * @brief create thread
 *
 * @param task task of thread
 * @param name thread name
 * @param entry thread user code function entry
 * @param arg args pass to entry
 * @return int status code
 */
static int kmt_create(task_t *task, const char *name, void (*entry)(void *arg),
                      void *arg) {
  return 0;
}

/**
 * @brief tear down a thread
 *
 * @param task task of thread
 */
static void kmt_teardown(task_t *task) {
}

Context *kmt_context_save(Event ev, Context *context) {
}
Context *kmt_context_switch(Event ev, Context *context) {
}
Context *kmt_timer(Event ev, Context *context) {
}
Context *kmt_error(Event ev, Context *context) {
}
Context *kmt_yield(Event ev, Context *context) {
}

MODULE_DEF(kmt) = {
    .init        = kmt_init,
    .create      = kmt_create,
    .teardown    = kmt_teardown,
    .spin_init   = spin_init,
    .spin_lock   = spin_lock,
    .spin_unlock = spin_unlock,
    .sem_init    = sem_init,
    .sem_wait    = sem_wait,
    .sem_signal  = sem_signal,
};
