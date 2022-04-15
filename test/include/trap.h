#ifndef __TRAP_H__
#define __TRAP_H__

#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <logger.h>

/**
 * @brief halt when condition `cond` is not true.
 */
__attribute__((noinline)) void _check(bool cond) {
  if (!cond) halt(1);
}

#define __QUOTE(str) #str
#define __STR(str)   __QUOTE(str)

#define check(cond) \
  putstr(FONT_BOLD "check: " FONT_NORMAL __STR(cond) "\n"); \
  _check((cond))

#endif