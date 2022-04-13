#include <trap.h>
#include <logger.h>

int main(int argc, char const *argv[]) {
  printf("log mask: %d\n", LOG_MASK);
  success("succeed!");
  info("info!");
  warn("warn!");
  error("error!");
  printf("");
  success_detail("succeed!");
  info_detail("info!");
  warn_detail("warn!");
  error_detail("error!");
  return 0;
}