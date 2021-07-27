#include "log.h"
#include "util.h"
#include "ContentGenerators.h"
#include <gtest/gtest.h>

TEST(regex, randomdevice) {
    tusion::util::RandomGenerator dev;
    std::string                   fet;
    dev.Generate(10, fet);
    log_info("string: {}", fet);
}

TEST(regex, regextest) {
    auto res = boost::regex_match("E{FAHUO}E{LIHUO}", boost::regex(tusion::gen::kPEnum));
    log_info("{}", res);
}

TEST(regex, rangemaker) {
    std::shared_ptr<tusion::gen::GeneratorRange> ptr_rg;
    bool                            res = tusion::gen::GetRange("range[50,70]", ptr_rg);
    log_info("Get Range Success: {}, Range: {},{}", res, ptr_rg->min,
             ptr_rg->max);
    for (auto i = 0; i < 10; i++) {
        size_t id;
        ptr_rg->generate(id);

        log_info("{}", id);
    }
}

TEST(regex, stringmaker) {
    std::shared_ptr<tusion::gen::GeneratorString> m;
    bool                             res = tusion::gen::GetString("string(2)", m);
    if (res) {
        log_info("{}", m->size);
        std::string s;
        for (auto i = 0; i < 10; i++) {
            m->generate(s);
            log_info("gen: {}", s);
        }
    }
}

TEST(regex, boolmaker) {
    std::shared_ptr<tusion::gen::GeneratorBool> m;
    bool                           res = tusion::gen::GetBool("bool", m);
    if (res) {
        for (auto i = 0; i < 10; i++) {
            bool s;
            m->generate(s);
            log_info("gen: {}", s);
        }
    }
}

TEST(regex, enumaker) {
    std::shared_ptr<tusion::gen::GeneratorEnum> ge;
    bool                           res = tusion::gen::GetEnum("E{FAHUO}E{LIHUO}", ge);
    if (res) {
        for (auto i = 0; i < 10; i++) {
            std::string s;
            ge->generate(s);
            log_info("gen: {}", s);
        }
    }
}

TEST(regex, intmaker) {
    std::shared_ptr<tusion::gen::GeneratorRange> m;
    bool                            res = tusion::gen::GetInt("int", m);
    if (res) {
        for (auto i = 0; i < 10; i++) {
            size_t s;
            m->generate(s);
            log_info("gen: {}", s);
        }
    }
}

TEST(regex, datemaker) {
    std::shared_ptr<tusion::gen::GeneratorDate> m;
    bool res = tusion::gen::GetDate("date", m, 2);
    if (res) {
        for (auto i = 0; i < 10; i++) {
            std::string datestr;
            m->serilize(datestr);
            log_info("gen: {}", datestr);
        }
    }
}

TEST(regex, maker) {
    std::string arr[] = {
        "range[3,12]",
        "string(14)",
        "bool",
        "int",
        "E{FAHUO}E{LIHUO}",
        "date",
        "@SrcVid",
    };
    for (const auto & i : arr) {
        auto p = tusion::gen::GetContentGenerator(i);
        std::string ctn;
        if (p.get()) {
            for (auto y = 0; y < 15; y++) {
                p->serilize(ctn);
                log_info("pattern: {}, y: {}, gen: {}", i, y, ctn);
            }
        } else {
            log_info("pattern: {}, not recognized.", i);
        }
        log_info("-----------");
    }
}

int main(int argc, char **argv) {
    tusion::util::init_module_console_log();
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
