
#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "log.h"

TEST(x, test) { log_info("hello!"); }

struct MyConfig2 {
    int x1;
    MyConfig2(const std::vector<int> &f) : x1(f.size()) {}
};

TEST(x, test1) {
    static std::vector<int> gVec;

    MyConfig2 mc(gVec = {1, 2, 3});
    log_info("{}", mc.x1);
}

int main(int argc, char **argv) {
    tusion::util::init_module_console_log();
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
