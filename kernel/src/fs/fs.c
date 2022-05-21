#include <devices.h>
#include <fs.h>
#include <file.h>

#define D "sda"
superblock_t sb;
spinlock_t fslock = {.lock_flag = false, .name = "fslock", .hold_cpuid = -1};

// Read block
void fs_readblk(device_t* dev, uint32_t blk_no, block_t* buf) {
  buf->blk_no = blk_no;
  dev->ops->read(dev, OFFSET_BLOCK(blk_no), buf->data, BSIZE);
}

// Write block
void fs_writeblk(device_t* dev, uint32_t blk_no, block_t* buf) {
  dev->ops->write(dev, OFFSET_BLOCK(blk_no), buf->data, BSIZE);
}

// set whole block to zero
void fs_zeroblk(device_t* dev, uint32_t blk_no) {
  block_t zero = {
      .blk_no = blk_no,
  };
  memset(zero.data, 0, sizeof(BSIZE));
  dev->ops->write(dev, OFFSET_BLOCK(blk_no), zero.data, BSIZE);
}

// alloc a free block(bit map 0),return block number
uint32_t fs_allocblk(device_t* dev) {
  uint32_t i, j, k;
  block_t block;
  for (i = 0; i < NBLOCK; i += BSIZE * 8) {
    block.blk_no = ROUNDUP_BLK_NUM((OFFSET_BITMAP(i)));
    // read bit map block
    dev->ops->read(dev, OFFSET_BITMAP(i), block.data, BSIZE);
    for (j = 0; j < BSIZE * 8 && i + j <= NBLOCK; j++) {
      // k is the byte that contains bitmap number j
      k = j / 8;
      // bit is 0 ,block is free
      if (!(block.data[k] & (1 << (7 - (j % 8))))) {
        // set bit to be used
        block.data[k] |= (1 << (7 - (j % 8)));
        dev->ops->write(dev, OFFSET_BITMAP(i + j), &(block.data[k]), 1);
        uint32_t blk_no = i + j;
        fs_zeroblk(dev, blk_no);
        return blk_no;
      }
    }
  }
  panic("[fs:fs_allocblk]:no free block");
}

// free an alloced block
void fs_freeblk(device_t* dev, uint32_t blk_no) {
  uint8_t byte;
  dev->ops->read(dev, OFFSET_BITMAP(blk_no), &byte, 1);
  byte = byte & ~(1 << (7 - (blk_no % 8)));
  dev->ops->write(dev, OFFSET_BITMAP(blk_no), &byte, 1);
}

// read superblock
void fs_readsb(device_t* dev, superblock_t* sb) {
  block_t block;
  dev->ops->read(dev, OFFSET_BOOT, block.data, BSIZE);
  memmove(sb, block.data, sizeof(*sb));
}

// create superblock
void fs_createsb(device_t* dev, superblock_t* sb) {
  dev->ops->write(dev, OFFSET_BOOT, sb, sizeof(*sb));
}

// initial superblock
void fs_init() {
  sb.nblocks    = NBLOCK;
  sb.ninodes    = NBLOCK;
  sb.inodestart = OFFSET_INODE(0) / BSIZE;
  sb.bmapstart  = OFFSET_BITMAP(0) / BSIZE;
  sb.size       = OFFSET_BLOCK(NBLOCK - 1);
  fs_createsb(dev->lookup(D), &sb);
}

// read an inode
void fs_readinode(device_t* dev, uint32_t inode_no, inode_t* inode) {
  inode->dev  = dev;
  inode->inum = inode_no;
  inode->ref  = 0;
  dev->ops->read(dev, OFFSET_INODE(inode_no), (&(inode->type)),
                 sizeof(dinode_t));
}

// write an inode
void fs_writeinode(device_t* dev, uint32_t inode_no, inode_t* inode) {
  // dinode_t* di = (dinode_t*)(&(inode->type));
  // printf("\nwrite dinode is "
  //        ":\n\tpos:%d\n\ttype:%d\n\tsize:%d\n\tnlinks:\n\taddrs:",
  //        OFFSET_INODE(inode_no), di->type, di->size, di->nlink);
  // for (int i = 0; i <= NDIRECT; i++) {
  //   printf("[%d] ", di->addrs[i]);
  // }
  // printf("\n");
  dev->ops->write(dev, OFFSET_INODE(inode_no), (dinode_t*)(&(inode->type)),
                  sizeof(dinode_t));
  // dev->ops->write(dev, OFFSET_INODE(inode_no), di, sizeof(dinode_t));
}

MODULE_DEF(fs) = {
    .init       = fs_init,
    .readblk    = fs_readblk,
    .writeblk   = fs_writeblk,
    .zeroblk    = fs_zeroblk,
    .allocblk   = fs_allocblk,
    .freeblk    = fs_freeblk,
    .readinode  = fs_readinode,
    .writeinode = fs_writeinode,
};