#include <ulib.h>
#include <utils.h>

void hello_test();
void dfs_test();

int main() {
  printf("Hello from user\n");
  dfs_test();
  hello_test();
  while (1)
    ;
}

void hello_test() {
  int pid = fork(), x = 0;

  const char *fmt;
  if (pid) {
    fmt = "Parent #%d\n";
  } else {
    sleep(1);
    fmt = "Child #%d\n";
  }

  while (1) {
    printf(fmt, ++x);
    sleep(2);
  }
}

#define DEST  'O'
#define EMPTY ' '

static struct move {
  int x, y, ch;
} moves[] = {
    {0, 1, '>'},
    {1, 0, 'v'},
    {0, -1, '<'},
    {-1, 0, '^'},
};

static char map[][16] = {"############",
                         "# #    #   #",
                         "# # ## # # #",
                         "# # #    # #",
                         "#     ## #O#",
                         "############",
                         ""};

void display();

void dfs(int x, int y) {
  if (map[x][y] == DEST) {
    cputstr("Found!\n");
  } else {
    display();
    sleep(1);
    int nfork = 0;

    for (struct move *m = moves; m < moves + 4; m++) {
      int x1 = x + m->x, y1 = y + m->y;
      if (map[x1][y1] == DEST || map[x1][y1] == EMPTY) {
        int pid = fork();
        if (pid == 0) {  // map[][] copied
          map[x][y] = m->ch;
          dfs(x1, y1);
        } else {
          nfork++;
          kputstr("forked\n");
        }
      }
    }
  }
  while (1)
    sleep(1);
}

void dfs_test() {
  dfs(1, 1);
  while (1)
    sleep(1);
}

void display() {
  for (int i = 0;; i++) {
    for (const char *s = map[i]; *s; s++) {
      char buf[3] = {*s, '\0'};
      if (*s == '#') buf[0] = 219;  // █
      write(1, buf, 1);
      // cputstr(buf);
      // switch (*s) {
      //   case EMPTY:
      //     cputstr("   ");
      //     break;
      //   case DEST:
      //     cputstr(" O ");
      //     break;
      //   case '>':
      //     cputstr(" > ");
      //     break;
      //   case '<':
      //     cputstr(" < ");
      //     break;
      //   case '^':
      //     cputstr(" ^ ");
      //     break;
      //   case 'v':
      //     cputstr(" V ");
      //     break;
      //   default:
      //     cputstr("###");
      //     break;
      // }
    }
    cputstr("\n");
    if (strlen(map[i]) == 0) break;
  }
}