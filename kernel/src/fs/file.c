#include <fs.h>
#include <file.h>

filetable_t ftable;

void file_init() {
  spin_init(&ftable.lock, "ftable_lock");
}

file_t* file_alloc() {
  file_t* f;
  spin_lock(&ftable.lock);
  for (f = ftable.files; f < ftable.files + FILE_TABLE_SIZE; f++) {
    if (f->ref == 0) {
      f->ref = 1;
      spin_unlock(&ftable.lock);
      return f;
    }
  }
  spin_unlock(&ftable.lock);
  warn("[file.c/file_alloc]:no file to alloc");
  return 0;
}

file_t* file_dup(file_t* f) {
  spin_lock(&ftable.lock);
  if (f->ref < 1) panic("[file.c/file_dup]:no ref");
  f->ref++;
  spin_unlock(&ftable.lock);
  return f;
}

int file_stat(file_t* f, stat_t* st) {
  if (f->type == FD_INODE) {
    spin_lock(&ftable.lock);
    st->type      = f->iptr->type;
    st->dev       = f->iptr->dev->id;
    st->inode_num = f->iptr->inum;
    st->links     = f->iptr->nlink;
    st->size      = f->iptr->size;
    spin_unlock(&ftable.lock);
    return 1;
  }
  return 0;
}

void file_close(file_t* f) {
  spin_lock(&ftable.lock);

  if (f->ref < 1) {
    panic("file already close!");
  }
  f->ref--;
  if (f->ref > 0) {
    spin_unlock(&ftable.lock);
  } else {
    f->type = FD_NONE;
    spin_unlock(&ftable.lock);
    // iput(f->iptr);
  }
}

//
int file_read(file_t* f, char* buf, uint32_t n) {
  int len=0;
  if (f->readable == 0) return -1;

  if (f->type == FD_INODE) {
    // len=readi(f->ip,buf,f->off,n);
    if (len > 0) f->off += len;
    return len;
  }
  panic("[file.c/file_read]read NULL");
}

int file_write(file_t* f, char* buf, uint32_t n) {
  int len=0;

  if (f->writable == 0) return -1;

  if (f->type == FD_INODE) {
    // len=writei(f->ip,buf,f->off,n);
    if (len > 0) f->off += len;
    return len;
  }
  panic("[file.c/file_write]write NULL");
}