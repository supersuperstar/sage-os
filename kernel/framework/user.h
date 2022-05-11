#define MAP_SHARED  1
#define MAP_PRIVATE 2
#define MAP_UNMAP   3

#define PROT_NONE  0x1
#define PROT_READ  0x2
#define PROT_WRITE 0x4

#include <stdint.h>

#define T_DIR  1
#define T_FILE 2

#define SEEK_CUR 0
#define SEEK_SET 1
#define SEEK_END 2

#define O_RDONLY 00000000
#define O_WRONLY 00000001
#define O_RDWR   00000002
#define O_CREAT  00000100

struct ufs_stat {
  uint32_t id, type, size;
};

struct ufs_dirent {
  uint32_t inode;
  char name[28];
} __attribute__((packed));
