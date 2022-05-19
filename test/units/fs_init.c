#include <kernel.h>
#include <klib.h>
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

  fs->init();
  block_t buf={
    .blk_no=0,
    .data="this is a test block data\n"
  };
  fs->writeblk(dev->lookup("sda"),0,&buf);
  block_t out;
  fs->readblk(dev->lookup("sda"),0,&out);
  printf("%s",out.data);
  
  // inode_t inode;
  // fs->readinode(dev->lookup("sda"),0,&inode);
  // printf("\ninode number is %d\n",inode.inum);


  mpe_init(os->run);

  while(1);
  return 1;
}