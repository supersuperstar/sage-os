#include <kernel.h>
#include <klib.h>

static void run() {
  printf("Hello World from CPU #%d\n", cpu_current());
  halt(0);
}

int main() {
  os->init();
  mpe_init(&run);
}