#include "log.h"
#include <string>
#include <gtest/gtest.h>

TEST(x, test) { log_info("hello!"); }

template <typename T>
struct base {
    virtual ~base() {}
    virtual void do_something(T& a) = 0;
};

struct implA : public base<int> {
    implA(int init) : initval_(new int(init)) {
    }

    virtual ~implA() {
        if (initval_) {
            delete initval_;
        }
        log_info("implA clear resource");
    }

    virtual void do_something(int &s) {
        s = 0;
    }
    int *initval_;
}

struct implB : public base <std::string> {
    implB(const std::string &initv) : initval_(initv) {}
    virtual ~implB = default;
    virtual void do_something (std::string &a) {
        a = initval_;
    }
    std::string initval_;
}

TEST(generate, inherit) {
}



int main(int argc, char **argv) {
    tusion::util::init_module_console_log();
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
