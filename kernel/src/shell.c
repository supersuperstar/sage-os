#include <common.h>
#include <shell.h>
#include <file.h>
#include <vfs.h>

const cmd_t cmd_list[] = {
  { "man",   man   },
  { "echo",   echo   },
  { "ls",     ls     },
  { "pwd",    pwd    },
  { "cd",     cd     },
  { "cat",    cat    },
  { "write",  write  },
  { "link",   link   },
  { "mkdir",  mkdir  },
  { "rmdir",  rmdir  },
  { "rm"   ,  rm     },
};
const int NR_CMD = sizeof(cmd_list) / sizeof(cmd_t);

void shell_task(task_t *proc) {
  char buf[512] = "";
  char pwd[512] = "";
  char cmd[512] = "";
  char ret[512] = "";

  sprintf(buf, "/dev/tty");
  int stdin = vfs->sys_open(proc, buf, O_RDONLY);
  int stdout = vfs->sys_open(proc, buf, O_WRONLY);
  Assert(stdin >= 0, "failed to open stdin");
  Assert(stdout >= 0, "failed to open stdout");

  sprintf(buf, "Welcome to sHELL.\nType [help] for help.\n\n");
  vfs->sys_write(proc, stdout, buf, strlen(buf));

  sprintf(pwd, "/");
  while (true) {
    sprintf(buf, "(tty) %s\n -> ",pwd);
    vfs->sys_write(proc, stdout, buf, strlen(buf));

    ssize_t nread = vfs->sys_read(proc, stdin, cmd, sizeof(cmd));
    cmd[nread - 1] = '\0';
    char *arg = cmd;
    while (*arg == ' ') ++arg;
    memset(ret, 0, sizeof(ret));

    bool succ = false;
    for (int i = 0; i < NR_CMD; ++i) {
      if (!strncmp(arg, cmd_list[i].name, strlen(cmd_list[i].name))) {
        succ = true;
        arg += strlen(cmd_list[i].name);
        while (*arg == ' ') ++arg;
        char *tmp = "cd";
        if(!strcmp("cd",cmd_list[i].name)) {
            cmd_list[i].func(proc, arg, pwd, ret);              
        } else  {            
            if(fork1(proc) == 0) {
                runcmd(proc, arg, pwd, ret,i);
            }
            int *stati;
            uproc->sys_wait(proc, stati);
      }
    }
    if (!succ) sprintf(ret, "Invalid command.\n");
    vfs->sys_write(proc, stdout, ret, strlen(ret));
    vfs->sys_write(proc, stdout, "\n", 1);
  }
  Panic("shell cannot exit.");
}

int fork1(task_t *proc) {
  int pid;

  pid = uproc->sys_fork(proc);
  if(pid == -1)
    panic("fork");
  return pid;
}

void runcmd(const char *arg, const char *pwd, char *ret,int i) {
    cmd_list[i].func(arg, pwd, ret);
    //todo:得到当前进程
    uproc->sys_exit();
}

bool get_dir(const char *arg, const char *pwd, char *dir) {
  char buf[512] = "";
  if (arg[0] == '/') {
    sprintf(buf, "%s", arg);
  } else {
    if (pwd[strlen(pwd) - 1] == '/') {
      sprintf(buf, "%s%s", pwd, arg);
    } else {
      sprintf(buf, "%s/%s", pwd, arg);
    }
  }

  size_t pos = 0;
  size_t cur = 0;
  size_t len = strlen(buf);
  while (pos <= len) {
    if (buf[pos] == ' ') {
      break;
    } else if (!strncmp(buf + pos, "//", 2)) {
      return false;
    } else if (!strncmp(buf + pos, "/./", 3)) {
      pos += 2;
      continue;
    } else if (!strncmp(buf + pos, "/../", 4)) {
      pos += 3;
      if (cur > 0 && dir[cur] == '/') --cur;
      while (cur > 0 && dir[cur] != '/') --cur;
      continue;
    } else {
      dir[cur] = buf[pos];
      ++pos, ++cur;
    }
  }
  dir[cur] = '\0';
  len = strlen(dir);
  if (len > 1 && dir[len - 1] == '/') dir[len - 1] = '\0';
  return true;
}

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
  char dir[512] = "";
  if (!get_dir(arg, pwd, dir)) {
    sprintf(ret, "Invalid directory address.\n");
  } else {
    vfs->sys_fstat(proc, dir, ret);
  }
}

FUNC(cd) {
  char dir[512] = "";
  if (!get_dir(arg, pwd, dir)) {
    sprintf(ret, "Invalid directory address.\n");
  } else {
    inode_t *ip = inode_search(root, dir);
    if (ip->type == TYPE_MNTP || ip->type == TYPE_DIRC) {
      strcpy(pwd, dir);
      sprintf(ret, "Directory changed to %s.\n", dir);
    } else {
      sprintf(ret, "Invalid inode type of %s: %s.\n", dir, inode_types_human[ip->type]);
    }
  }
}

FUNC(cat) {
  char dir[512] = "";
  if (!get_dir(arg, pwd, dir)) {
    sprintf(ret, "Invalid directory address.\n");
  } else {
    int fd = vfs->sys_open(proc, dir, O_RDONLY);
    if (fd < 0) {
      sprintf(ret, "VFS ERROR: open failed with status %d.\n"
          "Possible reasons:\n"
          " %d: command not supported by fs.\n"
          " %d: file does not exist.\n"
          " %d: file has wrong type.\n"
          " %d: file has wrong privilege.\n",
          fd, E_BADFS, E_NOENT, E_BADTP, E_BADPR);
    } else {
      vfs->sys_read(proc, fd, ret, 512);
      vfs->sys_close(proc, fd);
    }
  }
}

FUNC(write) {
  char dir[512] = "";
  char arg1[512] = "";
  const char *arg2 = arg;
  for (size_t i = 0; *arg2 != '\0'; ++i, ++arg2) {
    arg1[i] = *arg2;
    if (*arg2 == ' ') {
      arg1[i] = '\0';
      ++arg2;
      break;
    }
  }

  if (!get_dir(arg1, pwd, dir)) {
    sprintf(ret, "Invalid directory address.\n");
  } else {
    int fd = vfs->sys_open(proc, dir, O_WRONLY | O_CREAT);
    if (fd < 0) {
      sprintf(ret, "VFS ERROR: open failed with status %d.\n"
          "Possible reasons:\n"
          " %d: command not supported by fs.\n"
          " %d: file does not exist.\n"
          " %d: file has wrong type.\n"
          " %d: file has wrong privilege.\n",
          fd, E_BADFS, E_NOENT, E_BADTP, E_BADPR);
    } else {
      ssize_t nwrite = vfs->write(fd, (void *)arg2, strlen(arg2));
      vfs->sys_write(proc, fd, "\n", 1);
      vfs->sys_close(proc, fd);
      sprintf(ret, "Writed %d bytes successfully.\n", nwrite);
    }
  }
}


FUNC(link) {
  char dir1[512] = "";
  char dir2[512] = "";
  char arg1[512] = "";
  const char *arg2 = arg;
  for (size_t i = 0; *arg2 != '\0'; ++i, ++arg2) {
    arg1[i] = *arg2;
    if (*arg2 == ' ') {
      arg1[i] = '\0';
      ++arg2;
      break;
    }
  }
  if (!get_dir(arg1, pwd, dir1) || !get_dir(arg2, pwd, dir2)) {
    sprintf(ret, "Invalid directory address.\n");
  } else {
    int status = vfs->sys_link(proc, dir1, dir2);
    if (!status) {
      sprintf(ret, "Linked %s -> %s successfully.\n", dir1, dir2);
    } else {
      sprintf(ret, "VFS ERROR: link failed with status %d.\n", status);
      }
    }
  }
}

FUNC(mkdir) {
  char dir[512] = "";
  if (!get_dir(arg, pwd, dir)) {
    sprintf(ret, "Invalid directory address.\n");
  } else {
    int status = vfs->sys_mkdir(proc, dir);
    if (!status) {
      sprintf(ret, "Successfully created folder %s.\n", dir);
    } else {
      sprintf(ret, "VFS ERROR: mkdir failed with status %d.\n", status);
    } 
  }
}

FUNC(rmdir) {
  char dir[512] = "";
  if (!get_dir(arg, pwd, dir)) {
    sprintf(ret, "Invalid directory address.\n");
  } else {
    int status = vfs->sys_rmdir(proc, dir);
    if (!status) {
      sprintf(ret, "Successfully removed folder %s.\n", dir);
    } else {
      sprintf(ret, "VFS ERROR: rmdir failed with status %d.\n", status);
    } 
  }
}

FUNC(rm) {
  char dir[512] = "";
  if (!get_dir(arg, pwd, dir)) {
    sprintf(ret, "Invalid directory address.\n");
  } else {
    int status = vfs->sys_unlink(proc, dir);
    if (!status) {
      sprintf(ret, "Successfully removed %s.\n", dir);
    } else {
       sprintf(ret, "VFS ERROR: unlink failed with status %d.\n", status);
    }
  }
}
