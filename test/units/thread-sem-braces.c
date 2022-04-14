#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <sem.h>
#include <thread.h>
#include <logger.h>

#define MAX_COUNT 100

// notice: run with export smp=2
task_t *task_consumer, *task_producer;

int cnt = 0, tot = 0;
sem_t cnt_lock;
spinlock_t print_lock;

void consumer(void *arg) {
  while (1) {
    sem_wait(&cnt_lock);
    if (cnt) {
      cnt--;
      tot++;
      spin_lock(&print_lock);
      printf(")");
      spin_unlock(&print_lock);
      if (cnt == 0 && tot > MAX_COUNT) {
        sem_signal(&cnt_lock);
        break;
      }
    }
    sem_signal(&cnt_lock);
  }
  spin_lock(&print_lock);
  printf("C");
  spin_unlock(&print_lock);
  _log_mask = LOG_ERROR | LOG_WARN;
  while (1)
    ;
}

void producer(void *arg) {
  while (1) {
    sem_wait(&cnt_lock);
    if (tot > MAX_COUNT) {
      sem_signal(&cnt_lock);
      break;
    }
    cnt++;
    tot++;
    spin_lock(&print_lock);
    printf("(");
    spin_unlock(&print_lock);
    sem_signal(&cnt_lock);
  }
  spin_lock(&print_lock);
  printf("P");
  spin_unlock(&print_lock);
  while (1)
    ;
}

static void create_threads() {
  task_consumer = pmm->alloc(sizeof(task_t));
  task_producer = pmm->alloc(sizeof(task_t));
  kmt->create(task_consumer, "consumer", consumer, NULL);
  kmt->create(task_producer, "producer", producer, NULL);
}

int main() {
  ioe_init();

  _log_mask = LOG_ERROR | LOG_WARN | LOG_INFO;

  cte_init(os->trap);
  os->init();

  sem_init(&cnt_lock, "cnt_lock", 1);
  create_threads();
  kmt_print_all_tasks();
  kmt_print_cpu_tasks();
  mpe_init(os->run);
  return 1;
}