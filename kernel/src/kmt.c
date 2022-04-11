#include <common.h>
#include <sem.h>
#include <spinlock.h>
#include <thread.h>

task_t *kmt_get_task();
void kmt_set_task(task_t *task);
Context *kmt_context_save(Event ev, Context *context);
Context *kmt_yield(Event ev, Context *context);
Context *kmt_error(Event ev, Context *context);
Context *kmt_timer(Event ev, Context *context);
Context *kmt_schedule(Event ev, Context *context);

uint32_t next_pid = 1;  // next pid to allocate

const char *task_states_str[] = {"Unused",   "Embryo",  "To sleep", "Sleeping",
                                 "Waken up", "Running", "Zombie",   "Special"};

const char fence_val[32] = {
    FILL_FENCE, FILL_FENCE, FILL_FENCE, FILL_FENCE, FILL_FENCE, FILL_FENCE,
    FILL_FENCE, FILL_FENCE, FILL_FENCE, FILL_FENCE, FILL_FENCE, FILL_FENCE,
    FILL_FENCE, FILL_FENCE, FILL_FENCE, FILL_FENCE, FILL_FENCE, FILL_FENCE,
    FILL_FENCE, FILL_FENCE, FILL_FENCE, FILL_FENCE, FILL_FENCE, FILL_FENCE,
    FILL_FENCE, FILL_FENCE, FILL_FENCE, FILL_FENCE, FILL_FENCE, FILL_FENCE,
    FILL_FENCE, FILL_FENCE};

// extern spinlock_t os_trap_lock;

// special root point, do not execute
// all tasks are in root_task.list
task_t root_task;

// context to return when no task to schedule
Context *null_contexts[MAX_CPU] = {};

// current task in each cpu
task_t *cpu_tasks[MAX_CPU] = {};

/**
 * @brief check the fence protection value.
 *
 * @param task
 */
void check_fence(task_t *task) {
  assert(memcmp(fence_val, task->fenceA, sizeof(fence_val)) == 0);
  assert(memcmp(fence_val, task->fenceB, sizeof(fence_val)) == 0);
}

/**
 * @brief initialize kmt module
 *
 */
void kmt_init() {
  // create root_task
  root_task.pid   = next_pid++;
  root_task.name  = "Root Task";
  root_task.state = ST_X;
  root_task.count = 0;

  memset(root_task.fenceA, FILL_FENCE, sizeof(root_task.fenceA));
  memset(root_task.stack, FILL_STACK, sizeof(root_task.stack));
  memset(root_task.fenceB, FILL_FENCE, sizeof(root_task.fenceB));
  check_fence(&root_task);
  INIT_LIST_HEAD(&root_task.list);

  os->on_irq(INT32_MIN, EVENT_NULL, kmt_context_save);
  os->on_irq(0, EVENT_ERROR, kmt_error);
  os->on_irq(0, EVENT_IRQ_TIMER, kmt_timer);
  os->on_irq(0, EVENT_YIELD, kmt_yield);
  os->on_irq(INT32_MAX, EVENT_NULL, kmt_schedule);
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
int kmt_create(task_t *task, const char *name, void (*entry)(void *arg),
               void *arg) {
  return 0;
}

/**
 * @brief tear down a thread
 *
 * @param task task of thread
 */
void kmt_teardown(task_t *task) {
  // task_t *cur = kmt->get_task();
}

/**
 * @brief handler: save context to current task
 *
 * @param ev
 * @param context
 * @return Context* always NULL
 */
Context *kmt_context_save(Event ev, Context *context) {
  // task_t *cur = kmt->get_task();
  return NULL;
}

/**
 * @brief handler: handle yield interrupt (0x81)
 *
 * @param ev
 * @param context
 * @return Context* always NULL
 */
Context *kmt_yield(Event ev, Context *context) {
  return NULL;
}

/**
 * @brief handler: schedule next task to run
 *        Notice: this should be the only handler which returns real Context
 *
 * @param ev
 * @param context
 * @return Context*
 */
Context *kmt_schedule(Event ev, Context *context) {
  return context;
}

/**
 * @brief handler: triggered when aipc timer fired up
 *
 * @param ev
 * @param context
 * @return Context* always NULL
 */
Context *kmt_timer(Event ev, Context *context) {
  return NULL;
}

/**
 * @brief handler: handle program errors
 *
 * @param ev
 * @param context
 * @return Context* always NULL
 */
Context *kmt_error(Event ev, Context *context) {
  return NULL;
}

/**
 * @brief get current cpu's tasks
 *
 * @return task_t*
 */
task_t *kmt_get_task() {
  assert(cpu_current() < MAX_CPU);
  return cpu_tasks[cpu_current()];
}

void kmt_set_task(task_t *task) {
  assert(cpu_current() < MAX_CPU);
  cpu_tasks[cpu_current()] = task;
}

MODULE_DEF(kmt) = {
    .init        = kmt_init,
    .create      = kmt_create,
    .teardown    = kmt_teardown,
    .get_task    = kmt_get_task,
    .set_task    = kmt_set_task,
    .spin_init   = spin_init,
    .spin_lock   = spin_lock,
    .spin_unlock = spin_unlock,
    .sem_init    = sem_init,
    .sem_signal  = sem_signal,
    .sem_wait    = sem_wait,
};
