#include "log.h"
#include <gtest/gtest.h>

TEST(x, test) { log_info("hello!"); }



int main(int argc, char **argv) {
    tusion::util::init_module_console_log();
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
