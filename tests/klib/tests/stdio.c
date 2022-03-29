#include "trap.h"

int main() {
  char out[200];
  int friends=10;
  check(sprintf(out, "%d", 123) == 3);
  check(sprintf(out,"Hi,a nice day?Ahhhhhhh!!!")!=0);
  check(sprintf(out, "hello,I am %s,I have %d friends,they are at %p.", "Tom",friends,&friends)==54);
  check(sprintf(out, "hello,I am %s,I have %3.5d friends,they are at %p.", "Tom",friends,&friends)==57);

  return 0;
}
