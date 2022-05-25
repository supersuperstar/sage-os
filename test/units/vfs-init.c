#include <kernel.h>
#include <klib.h>
#include <user.h>
#include <am.h>
#include <logger.h>
#include <file.h>
#include <fs.h>

int main() {
  _log_mask = LOG_ERROR | LOG_INFO | LOG_WARN;
  ioe_init();
  cte_init(os->trap);
  os->init();
  vme_init(pmm->pgalloc, pmm->free);
  uproc->init();

  task_t* task     = pmm->alloc(sizeof(task_t));
  task->fdtable[0] = 0;
  task->fdtable[1] = 1;
  task->fdtable[2] = 2;
  for (int i = 3; i < PROCESS_FILE_TABLE_SIZE; i++) {
    task->fdtable[i] = -1;
  }
  vfs->init();

  //
  vfs->mkdir(task, "/usr/1");
  vfs->mkdir(task, "/usr/1");

  int fd = vfs->open(task, "/usr/1/te.c", O_CREAT | O_RDWR);
  // file_print_info(1);
  int fd3 = vfs->open(task, "/usr/1/te.c", O_CREAT | O_WRONLY);
  // file_print_info(1);
  char buf[50];
  memset(buf, 0, sizeof(buf));
  int w   = vfs->write(task, 3, "hello world!", 13);
  int fd2 = vfs->open(task, "/usr/1/te.c", O_RDONLY);
  // file_print_info(1);
  int r = vfs->read(task, 5, buf, 13);
  printf("%d %d %d %d %d\n", fd, fd3, w, fd2, r);
  printf("data is:%s\n", buf);
  vfs->close(task, 3);
  vfs->close(task, 4);
  vfs->close(task, 5);
  // file_print_info(1);

  // for(int i=0;i<10;i++){
  //   inode_print(i);
  // }

  fd = vfs->open(task, "/usr/1/te.c", O_CREAT | O_RDWR);
  // file_print_info(1);
  fd3 = vfs->open(task, "/usr/1/te.c", O_CREAT | O_WRONLY);
  // file_print_info(1);
  memset(buf, 0, sizeof(buf));
  w   = vfs->write(task, 3, "hello world version 2!", 23);
  fd2 = vfs->open(task, "/usr/1/te.c", O_RDONLY);
  // file_print_info(1);
  r = vfs->read(task, 4, buf, 23);
  printf("%d %d %d %d %d\n", fd, fd3, w, fd2, r);
  printf("data is:%s\n", buf);
  vfs->dup(task, 4);
  vfs->close(task, 3);
  vfs->close(task, 4);
  vfs->close(task, 5);
  file_print_info(1);

  while (1)
    ;
  // vfs->link(task,"","/uproc/task1");
  mpe_init(os->run);
  return 1;
}