#ifndef __THREAD_H__
#define __THREAD_H__

#include <common.h>
#include <list.h>

#define FILL_STACK 0xfd

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

  Context* context;
  enum task_states state;
  uint32_t count;

  char stack[8192];

  bool killed;

  struct list_head list;
};

const char* task_states_trans[];

#endif