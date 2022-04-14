#include <logger.h>

int _log_mask = LOG_MASK;

const char* logger_type_str[10] = {
    [LOG_SUCCESS] = FG_GREEN "SUCCESS" COLOR_NONE,
    [LOG_INFO]    = FG_CYAN "INFO" COLOR_NONE,
    [LOG_WARN]    = FG_PURPLE "WARN" COLOR_NONE,
    [LOG_ERROR]   = FG_RED "ERROR" COLOR_NONE,
};
