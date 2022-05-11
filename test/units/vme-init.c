#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <spinlock.h>
#include <thread.h>
#include <logger.h>
#include <devices.h>

int main() {
  ioe_init();

  _log_mask = LOG_ERROR | LOG_WARN | LOG_INFO | LOG_SUCCESS;

  cte_init(os->trap);
  os->init();
  mpe_init(os->run);
  return 1;
}