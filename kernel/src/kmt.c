#include <common.h>
#include <sem.h>
#include <spinlock.h>
#include <thread.h>

/**
 * @brief initialize kmt module
 *
 */
static void kmt_init() {
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
