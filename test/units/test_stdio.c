#include "trap.h"

int main() {
  char out[200];
  int friends=-10;
  snprintf(out,15,"Hello,I am %s,I have %05.3d friends.","Tom",friends);
  check(strcmp(out,"Hello,I am Tom")==0);
  sprintf(out,"Hello,I am %s,I have %05.3d friends.","Tom",friends);
  check(strcmp(out,"Hello,I am Tom,I have -00010 friends.")==0);
  return 0;
}
