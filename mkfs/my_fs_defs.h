#ifndef FS_DEFS_H__
#define FS_DEFS_H__

#include <stdint.h>

// #define O_RDONLY 00000000
// #define O_WRONLY 00000001
// #define O_RDWR   00000002
// #define O_CREAT  00000100

#define PATH_LENGTH 32

#define ROUNDUP_BLK_NUM(i) \
  (((i + BSIZE - 1) / BSIZE) * \
   BSIZE)  // offset of size (i) round up to block size's times
#define OFFSET_BOOT     (1024 * 1024)
#define OFFSET_SB       512
#define OFFSET_INODE(i) (OFFSET_BOOT + OFFSET_SB + (sizeof(dinode_t) * i))
#define OFFSET_ALLINODE \
  (OFFSET_BOOT + OFFSET_SB + ROUNDUP_BLK_NUM((sizeof(dinode_t) * NBLOCK)))
#define OFFSET_BITMAP(i) (OFFSET_ALLINODE + (i / 8))
#define OFFSET_ALLBITMAP (OFFSET_ALLINODE + NBLOCK / 8)
#define OFFSET_BLOCK(i)  (ROUNDUP_BLK_NUM(OFFSET_ALLBITMAP) + i * BSIZE)

#define NBLOCK        2048  // data block num
#define BSIZE         512   // block size
#define MAX_FILE_SIZE ((NDIRECT + NINDIRECT) * BSIZE)

#define DINODE_TYPE_N 0                           // unused
#define DINODE_TYPE_F 1                           // file
#define DINODE_TYPE_D 2                           // directory
#define PATH_LENGTH   32                          // path max length
#define NDIRECT       12                          // num of direct address
#define NINDIRECT     (BSIZE / sizeof(uint32_t))  // 128
#define MAXFILE       (NDIRECT + NINDIRECT)
#define ROOTINO       1

typedef struct {
  uint32_t size;        // Size of file system image (blocks)
  uint32_t nblocks;     // Number of data blocks
  uint32_t ninodes;     // Number of inodes.
  uint32_t inodestart;  // Block number of first inode block
  uint32_t bmapstart;   // Block number of first free map block
} superblock_t;

// On-disk inode structure
typedef struct {
  short type;                   // File type
  short nlink;                  // Number of links to inode in file system
  uint32_t size;                // Size of file (bytes)
  uint32_t addrs[NDIRECT + 1];  // Data block addresses
} dinode_t;

typedef struct {
  uint32_t blk_no;
  uint8_t data[BSIZE];
} block_t;

typedef struct {
  uint16_t inum;
  char name[PATH_LENGTH];
} dirent_t;

#endif