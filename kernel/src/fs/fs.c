#include <devices.h>
#include <fs.h>
#include <file.h>

#define D "sda"
superblock_t sb;
spinlock_t fslock = {.lock_flag = false, .name = "fslock", .hold_cpuid = -1};

// Read block
int fs_readblk(device_t* dev, uint32_t blk_no, block_t* buf) {
  buf->blk_no = blk_no;
  dev->ops->read(dev, OFFSET_BLOCK(blk_no), buf->data, BSIZE);
  return 0;
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

// alloc a free block(bit map 0)
uint32_t fs_allocblk(device_t* dev) {
  int i, j, k;
  block_t block;
  for (i = 0; i < NBLOCK; i += BSIZE) {
    block.blk_no = ROUNDUP(OFFSET_BITMAP(i), BSIZE);
    dev->ops->read(dev, OFFSET_BITMAP(i), block.data, BSIZE);
    for (j = 0; j < BSIZE && ((i + j) * 8 <= NBLOCK); j++) {
      for (k = sizeof(uint8_t) - 1; k >= 0; k--) {
        if (!(block.data[j] & (1 << k))) {  // bit is 0 ,block is free
          block.data[j] |= (1 << k);        // bit is used
          dev->ops->write(dev, OFFSET_BITMAP(i), block.data, BSIZE);
          uint32_t blk_no = (i + j) * 8;
          fs_zeroblk(dev, blk_no);
          return blk_no;
        }
      }
    }
  }
  panic("[fs:fs_allocblk]:no free block");
}

// free an alloced block
void fs_freeblk(device_t* dev, uint32_t blk_no) {
  uint8_t byte;
  dev->ops->read(dev, OFFSET_BITMAP(blk_no), &byte, sizeof(uint8_t));
  byte -= (byte & (1 << (sizeof(uint32_t) - 1 - (blk_no % sizeof(uint32_t)))));
  dev->ops->write(dev, OFFSET_BITMAP(blk_no), &byte, sizeof(uint8_t));
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
  // WARNING: overflow here
  // sb.size       = ((OFFSET_ALLBITMAP / BSIZE) + NBLOCK) * BSIZE;
  sb.size = 0;
}

void fs_readinode(device_t* dev, uint32_t inode_no, inode_t* inode) {
  dinode_t dinode;
  dev->ops->read(dev, OFFSET_INODE(inode_no), &dinode, sizeof(dinode_t));
  memcpy(&inode->type, &dinode, sizeof(dinode_t));
  inode->dev   = dev;
  inode->inum  = inode_no;
  inode->ref   = 0;
  inode->valid = 0;
}

void fs_writeinode(device_t* dev, uint32_t inode_no, inode_t* inode) {
  dev->ops->write(dev, OFFSET_INODE(inode_no), &(inode->type),
                  sizeof(dinode_t));
}

MODULE_DEF(fs) = {
    .init = fs_init,
};
