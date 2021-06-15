#include "log.h"

void say_hello() {
    tusion::util::init_module_console_log();
    log_info("hello!");
}
int main() {
    say_hello();
    return 0;
}
