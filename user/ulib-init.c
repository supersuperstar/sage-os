#include <ulib.h>

int main(int argc, char const *argv[]) {
  char s1[20] = "hello ";
  for (int i = 0; i < 5; i++) {
    int pid = fork();
    if (pid == 0) printf("#%d: %s %d\n", pid, s1, i);
  }
  return 0;
}
