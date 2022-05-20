#include <vfs.h>
#include <fs.h>
#include <file.h>

inode_t *itable;

void init() {
  int i;
  itable = pmm->alloc(sizeof(inode_t) * NBLOCK);
  // read all inode into memory
  for (i = 0; i < NBLOCK; i++) {
    fs->readinode(dev->lookup("sda"), i, itable + i);
  }
}

int write(int fd, void *buf, int count) {
}

int read(int fd, void *buf, int count) {
}

int close(int fd) {
}

int open(const char *pathname, int flags) {
}

int lseek(int fd, int offset, int whence) {
}

int link(const char *oldpath, const char *newpath) {
}

int unlink(const char *pathname) {
}

int fstat(int fd, struct ufs_stat *buf) {
}

int mkdir(const char *pathname) {
}

int chdir(const char *path) {
}

int dup(int fd) {
}

MODULE_DEF(vfs) = {.init   = init,
                   .write  = write,
                   .read   = read,
                   .close  = close,
                   .open   = open,
                   .lseek  = lseek,
                   .link   = link,
                   .unlink = unlink,
                   .fstat  = fstat,
                   .mkdir  = mkdir,
                   .chdir  = chdir,
                   .dup    = dup};