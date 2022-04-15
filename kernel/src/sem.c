#include <sem.h>

extern spinlock_t ir_lock;
extern task_t **cpu_tasks;
extern task_t root_task;

/**
 * @brief init a semaphore
 *
 * @param sem    // a semaphore instance
 * @param name   // initial name of instance
 * @param value  // initial value of instance
 */
void sem_init(sem_t *sem, const char *name, int value) {
  spin_init(&sem->lock, name);
  sem->name  = name;
  sem->value = value;
}

/**
 * @brief release semaphore(Mutex) and sleep task holding the sem
 *
 * @param sem a semaphore instance
 */
void sem_wait(sem_t *sem) {
  assert_msg(!spin_holding(&ir_lock), "do not allow sem_wait in trap");
  spin_lock(&sem->lock);

  while (sem->value <= 0) {
    assert_msg(!spin_holding(&ir_lock), "do not allow sleep in trap");

    spin_unlock(&sem->lock);
    spin_lock(&task_list_lock);
    task_t *cur = kmt->get_task();
    assert(cur);
    cur->wait_sem = sem;
    // interrupt
    spin_unlock(&task_list_lock);
    yield();
    spin_lock(&sem->lock);
  }

  __sync_synchronize();  // memory barrier
  --sem->value;
  spin_unlock(&sem->lock);
}

/**
 * @brief wake up a task form tasks waiting for the semaphore instance
 *
 * @param sem the semaphore instance
 */
void sem_signal(sem_t *sem) {
  spin_lock(&sem->lock);
  ++sem->value;
  spin_unlock(&sem->lock);

  bool holding = spin_holding(&task_list_lock);
  if (!holding) spin_lock(&task_list_lock);
  for (task_t *tp = root_task.next; tp != NULL; tp = tp->next) {
    if (tp->wait_sem == sem) {
      if (tp->state == ST_S) tp->state = ST_W;
      tp->wait_sem = NULL;  // stop going to sleep
    }
  }
  if (!holding) spin_unlock(&task_list_lock);
}