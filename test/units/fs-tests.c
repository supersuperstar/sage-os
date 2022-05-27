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

  //------------------------test  offset------------------------------------
  int o1, o2, o3, o4, o5, o6;
  o1 = OFFSET_BOOT;
  o6 = OFFSET_SB;
  o2 = OFFSET_ALLINODE;
  o3 = OFFSET_ALLBITMAP;
  o4 = OFFSET_BLOCK(0);
  o5 = OFFSET_BLOCK((NBLOCK));
  printf("OFFSET_BOOT is %d\n", o1);
  printf("OFFSET_ALLINODE is %d\n", o2);
  printf("OFFSET_ALLBITMAP is %d\n", o3);
  printf("OFFSET_BLOCK 0 is %d\n", o4);
  printf("OFFSET_BLOCK N is %d\n", o5);

  printf("------------------------\n");
  printf("|\tBOOT      \t|\n");
  printf("|   \t(%d)blocks\t|\n", o1 / BSIZE);
  printf("|end at (0x%8x)\t|\n", o1);
  printf("------------------------\n");
  printf("|\tSUPER BLOCK\t|\n");
  printf("|   \t(%d)blocks\t|\n", (o6) / BSIZE);
  printf("|end at (0x%8x)\t|\n", o1 + o6);
  printf("------------------------\n");
  printf("|\tINODE      \t|\n");
  printf("|   \t(%d)blocks\t|\n", (o2 - o6 - o1) / BSIZE);
  printf("|end at (0x%8x)\t|\n", o2);
  printf("------------------------\n");
  printf("|\tBIT MAP    \t|\n");
  printf("|   \t(%d)blocks\t|\n", (ROUNDUP_BLK_NUM(o3 - o2)) / BSIZE);
  printf("|end at (0x%8x)\t|\n", o3);
  printf("------------------------\n");
  printf("|\tDATA BLOCK\t|\n");
  printf("|   \t(%d)blocks\t|\n", (o5 - o4) / BSIZE);
  printf("|end at (0x%8x)\t|\n", o5);
  printf("------------------------\n");

  // //------------------------test data block rw------------------------------

  block_t buf;
  device_t* devs;
  char bufs[BSIZE * 20];
  devs = dev->lookup("sda");

  //---------------顺序读写------------------
  uint64_t start = safe_io_read(AM_TIMER_UPTIME).us;
  devs->ops->write(devs, OFFSET_BLOCK(30), bufs, BSIZE * 20);
  devs->ops->read(devs, OFFSET_BLOCK(30), bufs, BSIZE * 20);
  uint64_t end = safe_io_read(AM_TIMER_UPTIME).us;
  printf("RW 20 blocks time in order: %d ms\n", (end - start) / 1000);
  //---------------随机读写------------------
  uint64_t total = 0;
  for (int i = 1; i <= 20; i++) {
    sprintf((char*)buf.data, "this is data block %d.", i % 2 ? i : i + 100);
    start = safe_io_read(AM_TIMER_UPTIME).us;
    fs->writeblk(devs, i % 2 ? i : i + 100, &buf);
    end = safe_io_read(AM_TIMER_UPTIME).us;
    total += end - start;
  }
  block_t out;
  for (int i = 1; i <= 20; i++) {
    start = safe_io_read(AM_TIMER_UPTIME).us;
    fs->readblk(devs, i % 2 ? i : i + 100, &out);
    end = safe_io_read(AM_TIMER_UPTIME).us;
    total += end - start;
    // printf("data %d is :[%s]\n", i, out.data);
  }
  printf("RW 20 blocks time randomly: %d ms\n", total / 1000);

  //------------------------test inoderw-----------------------------------
  inode_t inode, inodeout;
  inode.dev   = dev->lookup("sda");
  inode.size  = 256;
  inode.inum  = 2;
  inode.nlink = 5;
  inode.type  = DINODE_TYPE_D;
  for (int i = 0; i < NDIRECT; i++) {
    inode.addrs[i] = i + 1;
  }
  fs->writeinode(dev->lookup("sda"), inode.inum, &inode);
  fs->readinode(dev->lookup("sda"), inode.inum, &inodeout);
  printf(
      "\ninode is:\n\tnum: %d\n\ttype: %d\n\tsize: %d\n\tnlinks:%d\n\taddrs:",
      inodeout.inum, inodeout.type, inodeout.size, inodeout.nlink);
  for (int i = 0; i <= NDIRECT; i++) {
    printf("[%d] ", inodeout.addrs[i]);
  }
  printf("\n");

  //------------------------test block alloc/free---------------------------
  int free[5] = {1, 3, 6, 9, 16};
  printf("OFFSET BITMAP:");
  for (int i = 0; i < 5; i++) {
    printf("%d(0x%8x) ", free[i], OFFSET_BITMAP(free[i]));
  }
  printf("\n");
  // alloc 20 block 0~19
  printf("alloc 20 block is :");
  for (int i = 0; i < 20; i++) {
    uint32_t blk_no = fs->allocblk(dev->lookup("sda"));
    printf(" %d", blk_no);
  }
  printf("\n");
  // free block 1,3,6,9,16
  for (int i = 0; i < 5; i++) {
    fs->freeblk(dev->lookup("sda"), free[i]);
  }
  // alloc again
  printf("alloc 20 block is :");
  for (int i = 0; i < 20; i++) {
    int blk_no = fs->allocblk(dev->lookup("sda"));
    printf(" %d", blk_no);
  }
  printf("\n");

  fs_print_datablock_bitmap_info(0);
  fs_print_inode_info(0);

  mpe_init(os->run);

  while (1)
    ;
  return 1;
}