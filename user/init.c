#include "ulib.h"

int main() {
  // Example:
  // printf("pid = %d\n", getpid());
  while (1) {
    kputstr("hello from initcode\n");
    for (int i = 0; i < 10000000; i++)
      ;
  }
  return 0;
}
