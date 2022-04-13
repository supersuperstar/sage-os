#include <sem.h>

extern spinlock_t ir_lock;
extern task_t **cpu_tasks;
extern task_t root_task;

void sem_init(sem_t *sem, const char *name, int value) {
  spin_init(&sem->lock, name);
  sem->name  = name;
  sem->value = value;
}

void sem_wait(sem_t *sem) {
  assert(!spin_holding(&ir_lock));
  spin_lock(&sem->lock);

  while (sem->value <= 0) {
    assert(!spin_holding(&ir_lock));

    spin_unlock(&sem->lock);
    spin_lock(&ir_lock);
    task_t *cur = kmt->get_task();
    assert(cur);
    cur->wait_sem = sem;
    spin_unlock(&ir_lock);
    yield();
    spin_lock(&sem->lock);
  }

  __sync_synchronize();
  --sem->value;
  spin_unlock(&sem->lock);
}

void sem_signal(sem_t *sem) {
  spin_lock(&sem->lock);
  ++sem->value;
  spin_unlock(&sem->lock);

  bool holding = spin_holding(&ir_lock);
  if (!holding) spin_lock(&ir_lock);
  for (task_t *tp = root_task.next; tp != NULL; tp = tp->next) {
    if (tp->wait_sem == sem) {
      if (tp->state == ST_S) tp->state = ST_W;
      tp->wait_sem = NULL;  // stop going to sleep
    }
  }
  if (!holding) spin_unlock(&ir_lock);
}