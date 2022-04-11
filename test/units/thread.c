#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <spinlock.h>
#include <thread.h>

task_t *task_consumer, *task_producer;

int cnt = 0;
spinlock_t *cnt_lock;

void consumer(void *arg) {
  while (1) {
    spin_lock(cnt_lock);
    if (cnt) {
      cnt--;
      printf(")");
    }
    spin_unlock(cnt_lock);
  }
}

void producer(void *arg) {
  while (1) {
    spin_lock(cnt_lock);
    cnt++;
    printf("(");
    spin_unlock(cnt_lock);
  }
}

static void create_threads() {
  task_consumer = pmm->alloc(sizeof(task_t));
  task_producer = pmm->alloc(sizeof(task_t));
  kmt->create(task_consumer, "consumer", consumer, NULL);
  kmt->create(task_producer, "producer", producer, NULL);
}

void mp_entry() {
  iset(true);
  yield();
}

int main() {
  ioe_init();
  cte_init(os->trap);
  os->init();
  create_threads();
  mpe_init(os->run);
  return 1;
}