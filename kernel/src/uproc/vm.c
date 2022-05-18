#include <kernel.h>
#include <thread.h>
#include <buddy.h>
#include <user.h>

/**
 * @brief map one physical page to one virtual page.
 *
 * @param proc PCB
 * @param vaddr virtual page addr
 * @param paddr physical page addr
 * @param prot protection bit
 */
void uproc_pgmap(AddrSpace* as, void* vaddr, void* paddr, int prot) {
  // TODO: need to record mapped pages for proc?
  uintptr_t va = (uintptr_t)vaddr;
  info("map va:0x%06x%06x -> pa:0x%x", va >> 24, va & ((1L << 24) - 1), paddr);
  // function map already has checks
  map(as, vaddr, paddr, prot);
}

void inituvm(AddrSpace* as, unsigned char* init, int sz) {
  assert_msg(sz <= SZ_PAGE, "initcode size greater than 4KB");
  char* mem;
  mem = pmm->pgalloc();
  memset(mem, 0, sizeof(mem));
  memcpy(mem, init, sz);
  uproc_pgmap(as, as->area.start, mem, MMAP_READ | MMAP_WRITE);
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
    uproc_pgmap(as, (void*)a, mem, MMAP_READ | MMAP_WRITE);
  }
  return newsz;
}

void copyuvm(AddrSpace* dst, AddrSpace* src, int sz) {
  protect(dst);
  intptr_t i;
  char* a;
  for (i = 0; i < sz; i += SZ_PAGE) {
    a = pmm->pgalloc();
    memcpy(a, (char*)(dst->area.start + i), SZ_PAGE);
    uproc_pgmap(dst, (void*)(dst->area.start + i), a,
                MMAP_READ | MMAP_WRITE);  // only read & write
  }
}

int deallocuvm(AddrSpace* as, int newsz, int oldsz) {
  assert(newsz <= oldsz);
  panic("not implemented");
}