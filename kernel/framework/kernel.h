#ifndef __KERNEL_H__
#define __KERNEL_H__
#include <am.h>

#define MODULE(mod) \
  typedef struct mod_##mod##_t mod_##mod##_t; \
  extern mod_##mod##_t *mod; \
  struct mod_##mod##_t

#define MODULE_DEF(mod) \
  extern mod_##mod##_t __##mod##_obj; \
  mod_##mod##_t *mod = &__##mod##_obj; \
  mod_##mod##_t __##mod##_obj

typedef Context *(*handler_t)(Event, Context *);
MODULE(os) {
  void (*init)();
  void (*run)();
  Context *(*trap)(Event ev, Context * context);
  void (*on_irq)(int seq, int event, handler_t handler);
};

MODULE(pmm) {
  void (*init)();
  void *(*alloc)(size_t size);
  void (*free)(void *ptr);
  void *(*pgalloc)();
};

typedef struct task task_t;
typedef struct spinlock spinlock_t;
typedef struct semaphore sem_t;

MODULE(kmt) {
  void (*init)();
  int (*create)(task_t * task, const char *name, void (*entry)(void *arg),
                void *arg);
  void (*teardown)(task_t * task);
  task_t *(*get_task)();
  void (*set_task)(task_t * task);
  void (*spin_init)(spinlock_t * lk, const char *name);
  void (*spin_lock)(spinlock_t * lk);
  void (*spin_unlock)(spinlock_t * lk);
  void (*sem_init)(sem_t * sem, const char *name, int value);
  void (*sem_wait)(sem_t * sem);
  void (*sem_signal)(sem_t * sem);
};

typedef struct device device_t;
MODULE(dev) {
  void (*init)();
  device_t *(*lookup)(const char *name);
};

MODULE(uproc) {
  void (*init)();
  int (*kputc)(task_t * task, char ch);
  int (*fork)(task_t * task);
  int (*wait)(task_t * task, int *status);
  int (*exit)(task_t * task, int status);
  int (*kill)(task_t * task, int pid);
  void *(*mmap)(task_t * task, void *addr, int length, int prot, int flags);
  int (*getpid)(task_t * task);
  int (*sleep)(task_t * task, int seconds);
  int64_t (*uptime)(task_t * task);
};

struct ufs_stat;
MODULE(vfs) {
  void (*init)();
  int (*write)(int fd, void *buf, int count);
  int (*read)(int fd, void *buf, int count);
  int (*close)(int fd);
  int (*open)(const char *pathname, int flags);
  int (*lseek)(int fd, int offset, int whence);
  int (*link)(const char *oldpath, const char *newpath);
  int (*unlink)(const char *pathname);
  int (*fstat)(int fd, struct ufs_stat *buf);
  int (*mkdir)(const char *pathname);
  int (*chdir)(const char *path);
  int (*dup)(int fd);
};


typedef struct inode inode_t;
typedef struct block block_t;
MODULE(fs) {
  void (*init)();
  void (*readblk)(device_t* dev, uint32_t blk_no, block_t* buf);
  void (*writeblk)(device_t* dev, uint32_t blk_no, block_t* buf);
  void (*zeroblk)(device_t* dev, uint32_t blk_no);
  uint32_t (*allocblk)(device_t* dev);
  void (*freeblk)(device_t* dev, uint32_t blk_no);
  void (*readinode)(device_t* dev, uint32_t inode_no, inode_t* inode);
  void (*writeinode)(device_t* dev, uint32_t inode_no, inode_t* inode);
};

#endif