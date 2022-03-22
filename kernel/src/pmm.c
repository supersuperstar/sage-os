#include <common.h>
#include <buddy.h>

struct pmm_pool global_mm_pool;

static void* kalloc(size_t size) {
  int npage               = (size + 1) / SZ_PAGE;
  int acquire_order       = power2ify(npage);
  struct chunk* page_addr = chunk_alloc(&global_mm_pool, acquire_order);
  if (page_addr != NULL) {
    return chunk2virt(&global_mm_pool, page_addr);
  }
  return NULL;
}

static void kfree(void* ptr) {
  struct chunk* chunk = virt2chunk(&global_mm_pool, ptr);
  chunk_free(&global_mm_pool, chunk);
}

static void pmm_init() {
  uintptr_t pmsize = ((uintptr_t)heap.end - (uintptr_t)heap.start);
  printf("Got %d MiB heap: [%p, %p)\n", pmsize >> 20, heap.start, heap.end);

  void* pg_start = NULL;
  void* pi_start = NULL;
  int nr_page;
  nr_page  = 128 * 1024;
  pg_start = heap.start;
  pi_start = (bool*)(pg_start + nr_page * SZ_PAGE);
  buddy_init(&global_mm_pool, pi_start, pg_start, nr_page);
}

MODULE_DEF(pmm) = {
    .init  = pmm_init,
    .alloc = kalloc,
    .free  = kfree,
};
