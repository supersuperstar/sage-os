#include <kernel.h>
#include <klib.h>
#include <klib-macros.h>

int power2ify(uint64_t size) {
  assert((int)size > 0);
  int order = 0;
  do {
    size >>= 1;
    order++;
  } while (size);
  return order - 1;
}
