#ifndef __THREAD_H__
#define __THREAD_H__

#include <common.h>
#include <list.h>

#define FILL_STACK 0xfd
#define FILL_FENCE 0xcd

enum task_states {
  ST_U,  // Unused
  ST_E,  // Embryo
  ST_T,  // To sleep
  ST_S,  // Sleeping
  ST_W,  // Waken up
  ST_R,  // Running
  ST_Z,  // Zombie
  ST_X   // Special
};

struct task {
  uint32_t pid;
  const char* name;
  void (*entry)(void*);
  void* arg;
  enum task_states state;
  sem_t* wait_sem;
  bool killed;
  uint32_t owner;
  uint32_t count;

  char fenceA[32];
  char stack[8192];
  char fenceB[32];
  Context* context;

  struct list_head list;
};

const char* task_states_trans[];

#endif