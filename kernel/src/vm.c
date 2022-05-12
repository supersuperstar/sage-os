#include <kernel.h>
#include <syscall.h>
#include <thread.h>
#include <buddy.h>
#include <user.h>

void inituvm(AddrSpace* as, char* init, int sz) {
  assert_msg(sz <= SZ_PAGE, "initcode size greater than 4KB");
  char* mem;
  mem = pmm->pgalloc();
  memset(mem, 0, sizeof(mem));
  map(as, 0, mem, MMAP_READ | MMAP_WRITE);
  memcpy(mem, init, sz);
}

int allocuvm(AddrSpace* as, int newsz, int oldsz) {
  assert(newsz >= oldsz);

  uintptr_t a = ROUNDUP(oldsz, SZ_PAGE);
  char* mem;
  for (; a < newsz; a += SZ_PAGE) {
    mem = pmm->pgalloc();
    if (mem == 0) {
      // ERROR
      // TODO:
    }
    memset(mem, 0, SZ_PAGE);
    map(as, a, mem, MMAP_READ | MMAP_WRITE);
  }
  return newsz;
}

AddrSpace copyuvm(AddrSpace* as, int sz) {
}

int deallocuvm(AddrSpace* as, int newsz, int oldsz) {
  assert(newsz <= oldsz);
}