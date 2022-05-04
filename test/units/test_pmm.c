#include <common.h>
#include <trap.h>
#include <logger.h>

int main() {
  pmm->init();
  void* alloc_addr[1024];
  for (int i = 0; i < 16; i++) {
    alloc_addr[i] = pmm->alloc(((i % 16) + 1) * 4096);
    check((intptr_t)alloc_addr[i] != 0);
    // success("%d allocation : 0x%x size: %dKB\n", i, alloc_addr[i],
    //         ((i % 16) + 1) * 4);
  }
  for (int i = 0; i < 16; i++) {
    pmm->free(alloc_addr[i]);
    // success("free %d: 0x%x\n", i, alloc_addr[i]);
  }
  return 0;
}