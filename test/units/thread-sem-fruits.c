/**
 * @file thread-sem-fruits.c
 * @author moeakwak (moeakwak@gmail.com)
 * @brief a semaphore concurrency demo, using multi processors
 * @version 0.2
 * @date 2022-04-15
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <sem.h>
#include <thread.h>
#include <logger.h>
#include <devices.h>

#define MAX_OFFER_TIMES 2
#define DAD_OFFER       2
#define MOM_OFFER       3

// notice: run with export smp>=4
task_t *task_dad, *task_mom, *task_son, *task_daughter;

sem_t plate, apple, orange;  // initial value: 1, 0, 0
spinlock_t print_lock;

void delay(int x) {
  for (long i = 0; i < x * 100000000; i++)
    ;
}

// dad put 2 apples on the plate
void dad(void *arg) {
  task_t *self = (task_t *)arg;
  int cnt      = 0;
  while (1) {
    sem_wait(&plate);
    spin_lock(&print_lock);
    cprintf("tty1", "CPU %d: *** Dad: offer %d apple, total offer: %d ***\n",
            cpu_current(), DAD_OFFER, (++cnt) * DAD_OFFER);
    if (cnt == MAX_OFFER_TIMES) {
      cprintf("tty1", "!!! Dad: exit !!\n");
      printf("!!! Dad: exit !!\n");
      kmt_print_all_tasks(LOG_INFO);
    }
    spin_unlock(&print_lock);
    for (int i = 0; i < DAD_OFFER; i++)
      sem_signal(&apple);
    if (cnt == MAX_OFFER_TIMES) {
      _log_mask = LOG_ERROR;
      kmt->teardown(self);
      while (1)
        ;
    }
    delay(10);
  }
}

// mom put 3 oranges on the plate
void mom(void *arg) {
  task_t *self = (task_t *)arg;
  int cnt      = 0;
  while (1) {
    sem_wait(&plate);
    spin_lock(&print_lock);
    cprintf("tty1", "CPU %d: *** Mom: offer %d oranges, total offer: %d ***\n",
            cpu_current(), MOM_OFFER, (++cnt) * MOM_OFFER);
    if (cnt == MAX_OFFER_TIMES) {
      cprintf("tty1", "!!! Mom: exit !!\n");
      printf("!!! Mom: exit !!\n");
      kmt_print_all_tasks(LOG_INFO);
    }
    spin_unlock(&print_lock);
    for (int i = 0; i < MOM_OFFER; i++)
      sem_signal(&orange);
    if (cnt == MAX_OFFER_TIMES) {
      _log_mask = LOG_ERROR;
      kmt->teardown(self);
      while (1)
        ;
    }
    delay(10);
  }
}

// son eat some oranges
void son(void *arg) {
  int cnt = 0;
  while (1) {
    sem_wait(&orange);
    spin_lock(&print_lock);
    cprintf("tty1", "CPU %d: *** Son: eat 1 orange, total: %d ***\n",
            cpu_current(), ++cnt);
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
    cprintf("tty1", "CPU %d: *** Daughter: eat 1 apple, total: %d ***\n",
            cpu_current(), ++cnt);
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
  kmt->create(task_dad, "task_dad", dad, task_dad);
  kmt->create(task_mom, "task_mom", mom, task_mom);
  kmt->create(task_son, "task_son", son, task_son);
  kmt->create(task_daughter, "task_daughter", daughter, task_daughter);
}

int main() {
  ioe_init();

  // _log_mask = LOG_ERROR | LOG_WARN | LOG_INFO | LOG_SUCCESS;
  _log_mask = LOG_ERROR | LOG_INFO;

  cte_init(os->trap);
  os->init();

  sem_init(&plate, "plate", 1);
  sem_init(&apple, "apple", 0);
  sem_init(&orange, "orange", 0);

  create_threads();
  kmt_print_all_tasks(LOG_INFO);
  mpe_init(os->run);
  return 1;
}