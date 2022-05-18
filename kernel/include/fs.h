#ifndef __FS_H__
#define __FS_H__

#include <common.h>
#include <sem.h>
#include <spinlock.h>

#define OFFSET_BOOT     1024 * 1024
#define OFFSET_SB       512
#define OFFSET_INODE(i) (OFFSET_BOOT + OFFSET_SB + (int)sizeof(dinode_t) * (i))
#define OFFSET_ALLINODE \
  (OFFSET_BOOT + OFFSET_SB + \
   ROUNDUP((int)sizeof(dinode_t) * NBLOCK, BSIZE) * BSIZE)
#define OFFSET_BITMAP(i) (OFFSET_ALLINODE + i / 8)
#define OFFSET_ALLBITMAP (OFFSET_ALLINODE + NBLOCK / 8)
#define OFFSET_BLOCK(i)  (OFFSET_ALLBITMAP + i * BSIZE)

#define NBLOCK 2 * 1024 * 1024  // data block num
#define BSIZE  512              // block size

#define DINODE_TYPE_F 0             // file
#define DINODE_TYPE_D 1             // directory
#define PATH_LENGTH   32            // path max length
#define NDIRECT       12            // num of direct address
#define NINDIRECT     (BSIZE / 32)  // 128
#define MAXFILE       (NDIRECT + NINDIRECT)
#define DIRSIZ        20

typedef struct superblock {
  uint32_t size;        // Size of file system image (blocks)
  uint32_t nblocks;     // Number of data blocks
  uint32_t ninodes;     // Number of inodes.
  uint32_t inodestart;  // Block number of first inode block
  uint32_t bmapstart;   // Block number of first free map block
} superblock_t;

// On-disk inode structure
typedef struct dinode {
  short type;                   // File type
  short nlink;                  // Number of links to inode in file system
  uint32_t size;                // Size of file (bytes)
  uint32_t addrs[NDIRECT + 1];  // Data block addresses
} dinode_t;

typedef struct block {
  uint32_t blk_no;
  uint8_t data[BSIZE];
} block_t;

typedef struct dirent {
  uint16_t inum;
  char name[DIRSIZ];
} dirent_t;

#endif
