#include "ulib.h"

void print_time() {
  int64_t t   = uptime();  // ms
  char st[10] = {(t / 10000) % 10 + '0', (t / 1000) % 10 + '0',
                 (t / 100) % 10 + '0',   (t / 10) % 10 + '0',
                 t % 10 + '0',           '\0'};
  kputstr(st);
}

int main() {
  while (1) {
    print_time();
    kputstr(" hello from initcode!\n");
    sleep(1);
  }
  return 0;
}
