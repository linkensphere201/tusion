#include "log.h"
#include <gtest/gtest.h>

TEST(x, test) { log_info("hello!"); }
#include <folly/futures/Future.h>
TEST(Future, isReady) {
    folly::Promise<int> p;
    auto f = p.getFuture();
    EXPECT_FALSE(f.isReady());
    p.setValue(42);
    EXPECT_TRUE(f.isReady());
}

int main(int argc, char **argv) {
    tusion::util::init_module_console_log();
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
