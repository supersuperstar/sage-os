#ifndef __SHELL_H__
#define __SHELL_H__

#define FUNC(name) void shell_##name(char *arg, char *pwd, char *ret)

typedef struct cmd {
  const char *name;
  void (*func)(char *arg, char *pwd, char *ret);
} cmd_t;

// bool get_dir(char *arg, char *pwd, char *dir);
int fork1();
void runcmd(char *arg, char *pwd, char *ret, int i);

FUNC(man);
FUNC(echo);
FUNC(pwd);
FUNC(ls);
FUNC(cd);
FUNC(cat);
FUNC(write);
FUNC(link);
FUNC(mkdir);
FUNC(rmdir);
FUNC(rm);
FUNC(run);
FUNC(ps);

#endif