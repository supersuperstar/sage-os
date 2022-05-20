#include "ulib.h"

void print_time() {
  int64_t t   = uptime();  // ms
  char st[10] = {(t / 10000) % 10 + '0', (t / 1000) % 10 + '0',
                 (t / 100) % 10 + '0',   (t / 10) % 10 + '0',
                 t % 10 + '0',           '\0'};
  kputstr(st);
}

int main() {
  // test fork
  int pid;
  for (int i = 0; i < 2; i++) {
    kputstr("main for loop\n");
    if ((pid = fork()) != 0) {
      sleep(1);
    } else {
      kputstr("subproc!\n");
    }
  }
  while (1)
    sleep(1000);
  return 0;
}
