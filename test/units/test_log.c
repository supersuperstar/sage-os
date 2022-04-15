#include <trap.h>
#include <logger.h>

void simple_test_log() {
  _log_mask = LOG_ERROR | LOG_WARN | LOG_SUCCESS | LOG_INFO;
  printf("log_mask: %d", _log_mask);
  printf("\n*******************************\n");
  printf("    Display log_mask: %02d  \n", _log_mask);
  printf("*******************************\n");
  success("succeed!");
  info("info!");
  warn("warn!");
  error("error!");
  printf("details:\n");
  success_detail("succeed!");
  info_detail("info!");
  warn_detail("warn!");
  error_detail("error!");
  _log_mask = LOG_ERROR | LOG_WARN | LOG_SUCCESS;
  printf("reset log_mask to %d, remove info\n", _log_mask);
  success("succeed!");
  info("info!");
  warn("warn!");
  error("error!");
}

int main(int argc, char const *argv[]) {
  simple_test_log();
  return 0;
}