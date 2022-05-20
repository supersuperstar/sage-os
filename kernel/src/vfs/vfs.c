#include <vfs.h>
#include <kernel.h>
#include <syscall_defs.h>
#include <syscalls.h>
#include <thread.h>
#include <fs.h>
#include <file.h>

inode_t *itable;

void vfs_init() {
  int i;
  itable = pmm->alloc(sizeof(inode_t) * NBLOCK);
  // read all inode into memory
  for (i = 0; i < NBLOCK; i++) {
    fs->readinode(dev->lookup("sda"), i, itable + i);
  }
}

int sys_open(task_t *proc, const char *pathname, int flags) {
  assert_msg(false, "sys_open not implemented");
  return 1;
}

int sys_close(task_t *proc, int fd) {
  assert_msg(false, "sys_close not implemented");
  return 1;
}

int sys_read(task_t *proc, int fd, void *buf, size_t nbyte) {
  assert_msg(false, "sys_read not implemented");
  return 1;
}

int sys_write(task_t *proc, int fd, void *buf, size_t nbyte) {
  assert_msg(false, "sys_write not implemented");
  return 1;
}

int sys_link(task_t *proc, const char *oldpath, const char *newpath) {
  assert_msg(false, "sys_link not implemented");
  return 1;
}

int sys_unlink(task_t *proc, const char *pathname) {
  assert_msg(false, "sys_unlink not implemented");
  return 1;
}

int sys_fstat(task_t *proc, int fd, struct ufs_stat *buf) {
  assert_msg(false, "sys_fstat not implemented");
  return 1;
}

int sys_mkdir(task_t *proc, const char *pathname) {
  assert_msg(false, "sys_mkdir not implemented");
  return 1;
}

int sys_chdir(task_t *proc, const char *path) {
  assert_msg(false, "sys_chdir not implemented");
  return 1;
}

int sys_dup(task_t *proc, int fd) {
  assert_msg(false, "sys_dup not implemented");
  return 1;
}

MODULE_DEF(vfs) = {.init   = vfs_init,
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