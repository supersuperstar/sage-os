#include <common.h>
#include <shell.h>
#include <user.h>
#include <fs.h>
#include <ulib.h>

const cmd_t cmd_list[] = {
    {"man", man},     {"echo", echo},   {"ls", ls},       {"pwd", pwd},
    {"cd", cd},       {"cat", cat},     {"write", write}, {"link", link},
    {"mkdir", mkdir}, {"rmdir", rmdir}, {"rm", rm},       {"run", run},
    {"ps", ps},
};
const int NR_CMD = sizeof(cmd_list) / sizeof(cmd_t);

void shell_task(void *arg) {
  task_t *proc  = (task_t *)arg;
  char buf[512] = "";
  char pwd[512] = "";
  char cmd[512] = "";
  char ret[512] = "";

  int stdin  = 0;
  int stdout = 1;

  sprintf(buf, "Welcome to SOShell.\nType [man] for help.\n\n");
  write(stdout, buf, strlen(buf));
  sprintf(pwd, "/");
  while (true) {
    sprintf(buf, "(tty) %s\n $ ", pwd);
    write(stdout, buf, strlen(buf));

    size_t nread   = read(stdin, cmd, sizeof(cmd));
    cmd[nread - 1] = '\0';
    // jump leading space
    char *arg = cmd;
    while (*arg == ' ')
      ++arg;
    memset(ret, 0, sizeof(ret));

    bool succ = false;
    for (int i = 0; i < NR_CMD; ++i) {
      if (!strncmp(arg, cmd_list[i].name, strlen(cmd_list[i].name))) {
        succ = true;
        arg += strlen(cmd_list[i].name);
        // skip interval spaces
        while (*arg == ' ')
          ++arg;
        // char *tmp = "cd";
        if (!strcmp("cd", cmd_list[i].name)) {
          cmd_list[i].func(proc, arg, pwd, ret);
        } else {
          if (fork1(proc) == 0) {
            task_t *nowproc = current_task;
            runcmd(nowproc, arg, pwd, ret, i);
          }
          int stati;
          wait(&stati);
        }
      }
      if (!succ) sprintf(ret, "Invalid command.\n");
      write(stdout, ret, strlen(ret));
      write(stdout, "\n", 1);
    }
  }
  panic("shell cannot exit.");
}

void shell_init() {
  task_t *task = pmm->alloc(sizeof(task_t));
  kmt->create(task, "shell", shell_task, task);
}

int fork1(task_t *proc) {
  int pid;

  pid = sys_fork(proc);
  if (pid == -1) panic("fork");
  return pid;
}

void runcmd(task_t *proc, char *arg, char *pwd, char *ret, int i) {
  cmd_list[i].func(proc, arg, pwd, ret);
  exit(0);
}

// bool get_dir(char *arg, char *pwd, char *dir) {
//   char buf[512] = "";
//   if (arg[0] == '/') {
//     sprintf(buf, "%s", arg);
//   } else {
//     if (pwd[strlen(pwd) - 1] == '/') {
//       sprintf(buf, "%s%s", pwd, arg);
//     } else {
//       sprintf(buf, "%s/%s", pwd, arg);
//     }
//   }

//   size_t pos = 0;
//   size_t cur = 0;
//   size_t len = strlen(buf);
//   while (pos <= len) {
//     if (buf[pos] == ' ') {
//       break;
//     } else if (!strncmp(buf + pos, "//", 2)) {
//       return false;
//     } else if (!strncmp(buf + pos, "/./", 3)) {
//       pos += 2;
//       continue;
//     } else if (!strncmp(buf + pos, "/../", 4)) {
//       pos += 3;
//       if (cur > 0 && dir[cur] == '/') --cur;
//       while (cur > 0 && dir[cur] != '/')
//         --cur;
//       continue;
//     } else {
//       dir[cur] = buf[pos];
//       ++pos, ++cur;
//     }
//   }
//   dir[cur] = '\0';
//   len      = strlen(dir);
//   if (len > 1 && dir[len - 1] == '/') dir[len - 1] = '\0';
//   return true;
// }

FUNC(man) {
  sprintf(ret, "Available commands: \n");
  for (int i = 0; i < NR_CMD; ++i) {
    strcat(ret, " - ");
    strcat(ret, cmd_list[i].name);
    strcat(ret, "\n");
  }
  strcat(ret, "\n");
}

FUNC(echo) {
  sprintf(ret, "%s\n", arg);
}

FUNC(pwd) {
  sprintf(ret, "%s\n", pwd);
}

FUNC(ls) {
  // TODO: ls
}

FUNC(cd) {
  if (!chdir(arg) != 0) {
    sprintf(ret, "Invalid directory address.\n");
  } else {
    sprintf(ret, "Change directory to %s.\n", arg);
  }
}

FUNC(cat) {
  int fd = open(arg, O_RDONLY);
  if (fd < 0) {
    sprintf(ret, "VFS ERROR: open failed with status %d.\n", fd);
  } else {
    read(fd, ret, sizeof(ret));
    close(fd);
  }
}

FUNC(write) {
  // char dir[512]  = "";
  char arg1[512] = "";
  char *arg2     = arg;
  for (size_t i = 0; *arg2 != '\0'; ++i, ++arg2) {
    arg1[i] = *arg2;
    if (*arg2 == ' ') {
      arg1[i] = '\0';
      ++arg2;
      break;
    }
  }

  int fd = open(arg1, O_WRONLY | O_CREAT);
  if (fd < 0) {
    sprintf(ret, "VFS ERROR: open failed with status %d.\n", fd);
  } else {
    size_t nwrite = write(fd, (void *)arg2, strlen(arg2));
    write(fd, "\n", 1);
    close(fd);
    sprintf(ret, "Writed %d bytes successfully.\n", nwrite);
  }
}

FUNC(link) {
  char arg1[512] = "";
  char *arg2     = arg;
  for (size_t i = 0; *arg2 != '\0'; ++i, ++arg2) {
    arg1[i] = *arg2;
    if (*arg2 == ' ') {
      arg1[i] = '\0';
      ++arg2;
      break;
    }
  }

  int status = link(arg1, arg2);
  if (!status) {
    sprintf(ret, "Linked %s -> %s successfully.\n", arg1, arg2);
  } else {
    sprintf(ret, "VFS ERROR: link failed with status %d.\n", status);
  }
}

FUNC(mkdir) {
  char dir[512] = "";

  int status = mkdir(arg);
  if (!status) {
    sprintf(ret, "Successfully created folder %s.\n", dir);
  } else {
    sprintf(ret, "VFS ERROR: mkdir failed with status %d.\n", status);
  }
}

FUNC(rmdir) {
  // char dir[512] = "";

  // int status = rmdir( arg);
  // if (!status) {
  //   sprintf(ret, "Successfully removed folder %s.\n", dir);
  // } else {
  //   sprintf(ret, "VFS ERROR: rmdir failed with status %d.\n", status);
  // }
  // TODO: rmdir
}

FUNC(rm) {
  int status = unlink(arg);
  if (!status) {
    sprintf(ret, "Successfully removed %s.\n", arg);
  } else {
    sprintf(ret, "VFS ERROR: unlink failed with status %d.\n", status);
  }
}

FUNC(run) {
}
FUNC(ps) {
}