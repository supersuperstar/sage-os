/**
 * @file thread-sem-braces.c
 * @author moeakwak (moeakwak@gmail.com)
 * @brief producer-consumer demo with semaphore
 * @version 0.1
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
      // printf(")");
      cprintf("tty1", ")");
      spin_unlock(&print_lock);
      if (cnt == 0 && tot > MAX_COUNT) {
        sem_signal(&cnt_lock);
        break;
      }
    }
    sem_signal(&cnt_lock);
  }
  spin_lock(&print_lock);
  // printf("C");
  cprintf("tty1", "C");
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
    cprintf("tty1", "(");
    spin_unlock(&print_lock);
    sem_signal(&cnt_lock);
  }
  spin_lock(&print_lock);
  printf("P");
  cprintf("tty1", "P");
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

  _log_mask = LOG_ERROR | LOG_WARN | LOG_INFO | LOG_SUCCESS;

  cte_init(os->trap);
  os->init();

  spin_init(&print_lock, "print_lock");
  sem_init(&cnt_lock, "cnt_lock", 1);

  create_threads();

  kmt_print_all_tasks(LOG_INFO);
  kmt_print_cpu_tasks(LOG_INFO);
  mpe_init(os->run);
  return 1;
}