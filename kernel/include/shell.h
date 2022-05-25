#ifndef __SHELL_H__
#define __SHELL_H__

#include "common.h"

#define FUNC(name) void name(const task_t *proc, char *arg, char *pwd, char *ret)

typedef struct cmd {
  const char *name;
  void (*func)(const task_t *proc, char *arg, char *pwd, char *ret);
} cmd_t;

void shell_task(task_t *proc);
bool get_dir(const char *arg, const char *pwd, char *dir);
int fork1(task_t *proc);
void runcmd(const task_t *proc,char *arg, const char *pwd, char *ret,int i)

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