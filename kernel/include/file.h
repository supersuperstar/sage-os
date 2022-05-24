#ifndef __FILE_H__
#define __FILE_H__

#include <common.h>
#include <sem.h>
#include <spinlock.h>

typedef struct file {
  enum { FD_NONE, FD_PIPE, FD_INODE } type;
  int ref; // reference count
  char readable;
  char writable;
  struct inode *iptr;
  uint32_t off;
} file_t;

// in-memory copy of an inode
typedef struct inode {
  device_t* dev;           // Device number
  uint32_t inum;          // Inode number
  int ref;            // Reference count
  spinlock_t lock; // protects everything below here
  int valid;          // inode has been read from disk?

  short type;         // copy of disk inode
  short nlink;
  uint32_t size;
  uint32_t addrs[NDIRECT+1];
} inode_t;

#endif