#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <spinlock.h>
#include <thread.h>
#include <logger.h>

#define MAX_TASK      4
#define RUNNING_COUNT 2

task_t *tasks[MAX_TASK];
char names[10][MAX_TASK];


int cnt = MAX_TASK;  // current running thread count
spinlock_t cnt_lock, print_lock;


void func(void *arg) {
  task_t *self = (task_t *)arg;

  for (int i = 0; i < RUNNING_COUNT; i++) {
    // print something
    spin_lock(&print_lock);
    warn("%s running %d times, count=%d", self->name, i, self->count);
    kmt_print_cpu_tasks(LOG_INFO);
    printf("\n");
    spin_unlock(&print_lock);
    // give up cpu to reschedule
    yield();
  }

  // if finished
  spin_lock(&print_lock);
  warn("%s exit!", self->name);
  kmt_print_all_tasks(LOG_INFO);
  kmt_print_cpu_tasks(LOG_INFO);
  spin_unlock(&print_lock);

  int remain = 0;
  spin_lock(&cnt_lock);
  remain = --cnt;
  spin_unlock(&cnt_lock);
  if (remain >= cpu_count()) {
    kmt->teardown(self);
    yield();
  }
  _log_mask = LOG_INFO | LOG_WARN | LOG_ERROR;
  while (1)
    ;
}

static void create_threads() {
  for (int i = 0; i < MAX_TASK; i++) {
    tasks[i] = pmm->alloc(sizeof(task_t));
    sprintf(names[i], "T%02d\0", i);
    kmt->create(tasks[i], names[i], func, tasks[i]);
  }
}

int main() {
  ioe_init();

  _log_mask = LOG_INFO | LOG_WARN | LOG_ERROR | LOG_SUCCESS;

  cte_init(os->trap);
  os->init();

  spin_init(&print_lock, "print_lock");
  spin_init(&cnt_lock, "cnt_lock");

  create_threads();
  kmt_print_all_tasks(LOG_INFO);
  kmt_print_cpu_tasks(LOG_INFO);
  mpe_init(os->run);
  return 1;
}