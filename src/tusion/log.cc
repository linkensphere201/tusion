#include "log.h"
#include <spdlog/sinks/stdout_color_sinks.h>


namespace tusion {
namespace util {
std::shared_ptr<spdlog::logger> g_console;
void init_module_console_log() {
    g_console = spdlog::stdout_color_mt("console");
    spdlog::set_default_logger(g_console);
    spdlog::set_level(spdlog::level::debug);
    spdlog::set_pattern("%Y-%m-%d %H:%M:%S.%e [thread %t] %l %v");
}
} // namespace util
} // namespace tusion
