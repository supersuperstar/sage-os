#ifndef __TRAP_H__
#define __TRAP_H__

#include <am.h>
#include <klib.h>
#include <klib-macros.h>

/**
 * @brief halt when condition `cond` is not true.
 */
__attribute__((noinline)) void check(bool cond) {
  if (!cond) halt(1);
}

#endif