#include <trap.h>
#include <logger.h>

void simple_test_log() {
  printf("mask: %d", _log_mask);
  success("succeed!");
  info("info!");
  warn("warn!");
  error("error!");
  printf("details:\n");
  success_detail("succeed!");
  info_detail("info!");
  warn_detail("warn!");
  error_detail("error!");
  _log_mask = 13;
  printf("reset log_mask to %d, remove info\n", _log_mask);
  success("succeed!");
  info("info!");
  warn("warn!");
  error("error!");
}

int main(int argc, char const *argv[]) {
    return 0;
}