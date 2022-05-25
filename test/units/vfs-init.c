#include <kernel.h>
#include <klib.h>
#include <user.h>
#include <am.h>
#include <logger.h>
#include <file.h>
#include <fs.h>

int main() {
  _log_mask = LOG_ERROR | LOG_INFO;
  ioe_init();
  cte_init(os->trap);
  os->init();
  vme_init(pmm->pgalloc, pmm->free);
  uproc->init();

  vfs->init();

  task_t* task=pmm->alloc(sizeof(task_t));
  task->fdtable[0]=0;
  task->fdtable[1]=1;
  task->fdtable[2]=2;
  vfs->open(task,"/usr/te.c",O_CREAT | O_RDWR);
  //vfs->link(task,"","/uproc/task1");
  mpe_init(os->run);
  return 1;
}