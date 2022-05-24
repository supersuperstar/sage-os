#include "ulib.h"

void print_time() {
  int64_t t   = uptime();  // ms
  char st[10] = {(t / 10000) % 10 + '0', (t / 1000) % 10 + '0',
                 (t / 100) % 10 + '0',   (t / 10) % 10 + '0',
                 t % 10 + '0',           '\0'};
  kputstr(st);
}

int main() {
  // int fd       = open("/dev/zero", O_RDONLY);
  // char buf[10] = {-1};
  // // will assert fault here: read not implemented
  // if (read(fd, buf, 1) != -1 && buf[0] == 0) {
  //   kputstr("success!");
  // } else {
  //   kputstr("error!");
  // }

  while (1) {
    print_time();
    kputstr(" hello from initcode!\n");
    // test open
    sleep(1);
  }
  return 0;
}
