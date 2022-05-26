#include <kernel.h>
#include <klib.h>
#include <user.h>
#include <am.h>
#include <logger.h>
#include <file.h>
#include <fs.h>

void func(void* arg) {
  task_t* task = (task_t*)arg;
  vfs->mkdir(task, "usr");
  vfs->mkdir(task, "/usr/1");
  vfs->mkdir(task, "/usr/1");

  int fd = vfs->open(task, "/usr/1/te.c", O_CREAT | O_RDWR);
  file_print_info(1);
  int fd3 = vfs->open(task, "/usr/1/te.c", O_CREAT | O_WRONLY);
  file_print_info(1);
  char buf[50];
  memset(buf, 0, sizeof(buf));
  int w   = vfs->write(task, 3, "hello world!", 13);
  int fd2 = vfs->open(task, "/usr/1/te.c", O_RDONLY);
  file_print_info(1);
  vfs->lseek(task, 3, 0);
  int r = vfs->read(task, 3, buf, 13);
  printf("%d %d %d %d %d\n", fd, fd3, w, fd2, r);
  printf("data is:%s\n", buf);
  vfs->close(task, 3);
  vfs->close(task, 4);
  vfs->close(task, 5);
  file_print_info(1);

  // for(int i=0;i<10;i++){
  //   inode_print(i);
  // }

  fd = vfs->open(task, "/usr/1/te.c", O_CREAT | O_RDWR);
  file_print_info(1);
  fd3 = vfs->open(task, "/usr/1/te.c", O_CREAT | O_WRONLY);
  file_print_info(1);
  memset(buf, 0, sizeof(buf));
  w   = vfs->write(task, 3, "hello world version 2!", 23);
  fd2 = vfs->open(task, "/usr/1/te.c", O_RDONLY);
  file_print_info(1);
  r = vfs->read(task, 4, buf, 23);
  printf("%d %d %d %d %d\n", fd, fd3, w, fd2, r);
  printf("data is:%s\n", buf);
  vfs->dup(task, 4);
  vfs->close(task, 3);
  vfs->close(task, 4);
  vfs->close(task, 5);
  file_print_info(1);

  fd=vfs->open(task,"/dev/random",O_CREAT);
  r=vfs->read(task,fd,0,0);
  printf("read from RANDOM is %d\n",r);
  r=vfs->mkdir(task,"/dev/input");
  printf("mkdri result is %d\n",r);

  while (1)
    ;
  // vfs->link(task,"","/uproc/task1");
}

int main() {
  _log_mask = LOG_ERROR | LOG_INFO | LOG_WARN;
  ioe_init();
  cte_init(os->trap);
  os->init();
  vme_init(pmm->pgalloc, pmm->free);
  uproc->init();
  vfs->init();

  task_t* task = pmm->alloc(sizeof(task_t));
  kmt->create(task, "test", func, task);

  mpe_init(os->run);
  return 1;
}