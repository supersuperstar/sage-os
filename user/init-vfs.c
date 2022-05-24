#include <ulib.h>

void print_time() {
  int64_t t   = uptime();  // ms
  char st[10] = {(t / 10000) % 10 + '0', (t / 1000) % 10 + '0',
                 (t / 100) % 10 + '0',   (t / 10) % 10 + '0',
                 t % 10 + '0',           '\0'};
  kputstr(st);
}

int cputstr(char* s, size_t len) {
  return write(0, s, len);
}

char getc() {
  char ch;
  if (read(1, &ch, 1) != -1) {
    return ch;
  } else {
    return '\0';
  }
}

int main() {
  char buf[] = "*<-Your input";
  cputstr("hello!\n", 8);
  buf[0] = getc();
  cputstr(buf, 14);
  while (1) {
    print_time();
    kputstr(" hello from initcode!\n");
    // test open
    sleep(1);
  }
  return 0;
}
