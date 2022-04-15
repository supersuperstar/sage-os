#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <sem.h>
#include <thread.h>
#include <logger.h>
#include <devices.h>

#define MAX_OFFER 5

// notice: run with export smp=4
task_t *task_dad, *task_mom, *task_son, *task_daughter;

sem_t plate, apple, orange;  // initial value: 1, 0, 0
spinlock_t print_lock;

void delay(int x) {
  for (long i = 0; i < x * 100000000; i++)
    ;
}

// dad put an apple on the plate
void dad(void *arg) {
  int cnt = 0;
  while (1) {
    sem_wait(&plate);
    spin_lock(&print_lock);
    printf("CPU %d: *** Dad: offer apple %d ***\n", cpu_current(), ++cnt);
    cprintf("tty1", "CPU %d: *** Dad: offer apple %d ***\n", cpu_current(),
            cnt);
    if (cnt == MAX_OFFER) {
      printf("!!! Dad: exit !!\n");
    }
    spin_unlock(&print_lock);
    sem_signal(&apple);
    if (cnt == MAX_OFFER) {
      while (1)
        ;
    }
    delay(10);
  }
}

// mom put an orange on the plate
void mom(void *arg) {
  int cnt = 0;
  while (1) {
    sem_wait(&plate);
    spin_lock(&print_lock);
    printf("CPU %d: *** Mom: offer orange %d ***\n", cpu_current(), ++cnt);
    cprintf("tty1", "CPU %d: *** Mom: offer orange %d ***\n", cpu_current(),
            cnt);
    if (cnt == MAX_OFFER) {
      printf("!!! Mom: exit !!\n");
    }
    spin_unlock(&print_lock);
    sem_signal(&orange);
    if (cnt == MAX_OFFER) {
      while (1)
        ;
    }
    delay(1);
  }
}

// son eat one orange
void son(void *arg) {
  int cnt = 0;
  while (1) {
    sem_wait(&orange);
    spin_lock(&print_lock);
    printf("CPU %d: *** Son: take orange %d ***\n", cpu_current(), ++cnt);
    cprintf("tty1", "CPU %d: *** Son: take orange %d ***\n", cpu_current(),
            cnt);
    spin_unlock(&print_lock);
    sem_signal(&plate);
    delay(10);
  }
}

// daughter eat one apple
void daughter(void *arg) {
  int cnt = 0;
  while (1) {
    sem_wait(&apple);
    spin_lock(&print_lock);
    printf("CPU %d: *** Daughter: take apple %d ***\n", cpu_current(), ++cnt);
    cprintf("tty1", "CPU %d: *** Daughter: take apple %d ***\n", cpu_current(),
            cnt);
    spin_unlock(&print_lock);
    sem_signal(&plate);
    delay(3);
  }
}

static void create_threads() {
  task_dad      = pmm->alloc(sizeof(task_t));
  task_mom      = pmm->alloc(sizeof(task_t));
  task_son      = pmm->alloc(sizeof(task_t));
  task_daughter = pmm->alloc(sizeof(task_t));
  kmt->create(task_dad, "task_dad", dad, NULL);
  kmt->create(task_mom, "task_mom", mom, NULL);
  kmt->create(task_son, "task_son", son, NULL);
  kmt->create(task_daughter, "task_daughter", daughter, NULL);
}

int main() {
  ioe_init();

  _log_mask = LOG_ERROR;

  cte_init(os->trap);
  os->init();

  sem_init(&plate, "plate", 1);
  sem_init(&apple, "apple", 0);
  sem_init(&orange, "orange", 0);

  create_threads();
  kmt_print_all_tasks();
  kmt_print_cpu_tasks();
  mpe_init(os->run);
  return 1;
}