#include <common.h>
#include <sem.h>
#include <spinlock.h>
#include <thread.h>
#include <limits.h>

// static task_t *kmt_get_task();
// static void kmt_set_task(task_t *task);
static Context *kmt_context_save(Event ev, Context *context);
static Context *kmt_yield(Event ev, Context *context);
static Context *kmt_schedule(Event ev, Context *context);

/**
 * @brief initialize kmt module
 *
 */
static void kmt_init() {
  // TODO: initialize tasks

  os->on_irq(INT_MIN, EVENT_NULL, kmt_context_save);
  os->on_irq(0, EVENT_YIELD, kmt_yield);
  os->on_irq(INT_MAX, EVENT_NULL, kmt_schedule);
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
  // task_t *cur = kmt->get_task();
}

/**
 * @brief save context to current task
 *
 * @param ev
 * @param context
 */
static Context *kmt_context_save(Event ev, Context *context) {
  // task_t *cur = kmt->get_task();
  return NULL;
}

static Context *kmt_yield(Event ev, Context *context) {
  return NULL;
}

static Context *kmt_schedule(Event ev, Context *context) {
  return context;
}

MODULE_DEF(kmt) = {
    .init     = kmt_init,
    .create   = kmt_create,
    .teardown = kmt_teardown,
    // .get_task = kmt_get_task,
    // .set_task = kmt_set_task,
};
