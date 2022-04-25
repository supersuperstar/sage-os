#include <kernel.h>
#include <klib.h>

int main() {
  ioe_init();
  cte_init(os->trap);
  os->init();
  vme_init(pmm->pgalloc, pmm->free);
  mpe_init(os->run);
  return 1;
}
