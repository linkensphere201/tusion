#include "spdlog/spdlog.h"

namespace tusion {
namespace util {
extern std::shared_ptr<spdlog::logger> g_console;
void init_module_console_log();
} // namespace util
} // namespace tusion

#define do_log(LLVL, LOGGER, VAL, ...)                                         \
    if (LOGGER) {                                                              \
        LOGGER->LLVL("[{}:{}] " VAL, __FILE__, __LINE__, ##__VA_ARGS__);       \
    }

#define log_warn(VAL, ...)                                                     \
    do_log(warn, tusion::util::g_console.get(), VAL, ##__VA_ARGS__);

#define log_error(VAL, ...)                                                    \
    do_log(error, tusion::util::g_console.get(), VAL, ##__VA_ARGS__);

#define log_info(VAL, ...)                                                     \
    do_log(info, tusion::util::g_console.get(), VAL, ##__VA_ARGS__);

#define log_debug(VAL, ...)                                                    \
    do_log(debug, tusion::util::g_console.get(), VAL, ##__VA_ARGS__);
