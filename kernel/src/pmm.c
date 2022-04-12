#include <common.h>
#include <buddy.h>
#include <logger.h>

struct pmm_pool global_mm_pool;

static void* kalloc(size_t size) {
  int npage         = (size + 1) / SZ_PAGE;
  int acquire_order = power2ify(npage);
  log_detail(LOG_INFO, "acquire order: %d", acquire_order);
  struct chunk* page_addr = chunk_alloc(&global_mm_pool, acquire_order);
  if (page_addr != NULL) {
    log_detail(LOG_INFO, "allocate addr: %#x",
               chunk2virt(&global_mm_pool, page_addr));
    return chunk2virt(&global_mm_pool, page_addr);
  }
  warn("fail to alloc addr");
  return NULL;
}

static void* kalloc_safe(size_t size) {
  bool i = ienabled();
  iset(false);
  void* ret = kalloc(size);
  if (i) iset(true);
  return ret;
}

static void kfree(void* ptr) {
  struct chunk* chunk = virt2chunk(&global_mm_pool, ptr);
  chunk_free(&global_mm_pool, chunk);
  info_detail("free successfully, address: %#x", ptr);
}

static void kfree_safe(void* ptr) {
  int i = ienabled();
  iset(false);
  kfree(ptr);
  if (i) iset(true);
}

static void pmm_init() {
  uintptr_t pmsize = ((uintptr_t)heap.end - (uintptr_t)heap.start);
  printf("Got %d MiB heap: [%p, %p)\n", pmsize >> 20, heap.start, heap.end);

  void* pg_start = NULL;
  void* pi_start = NULL;
  int nr_page;
  nr_page =
      (uint64_t)(heap.end - heap.start) / (SZ_PAGE + sizeof(struct chunk));
  pg_start = heap.start;
  pi_start = (bool*)(pg_start + nr_page * SZ_PAGE);
  // global_mm_pool = pi_start;
  // pi_start       = (struct pmm_pool*)pi_start + 1;
  log_detail(
      LOG_INFO,
      "page start addr: %#x, page indicator addr: %#x, available page: %d",
      pg_start, pi_start, nr_page);
  log_detail(LOG_INFO, "begin to init buddy system");
  buddy_init(&global_mm_pool, pi_start, pg_start, nr_page);
}

MODULE_DEF(pmm) = {
    .init  = pmm_init,
    .alloc = kalloc_safe,
    .free  = kfree_safe,
};
