#ifndef __COMMON_H__
#define __COMMON_H__

#include <kernel.h>
#include <klib.h>
#include <klib-macros.h>

#define MAX_CPU 8

static inline int power2ify(uint64_t size) {
  assert((int)size > 0);
  int order = 0;
  do {
    size >>= 1;
    order++;
  } while (size);
  return order - 1;
}

#endif
