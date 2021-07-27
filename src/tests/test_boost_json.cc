#include "log.h"
#include <gtest/gtest.h>


#include <string>
#include <fstream>
#include <iostream>
#include <boost/json/src.hpp>

TEST(json, basic) {
    std::string configfile = "sample.json";
    std::ifstream ifs {configfile};
    std::ostringstream sstr;
    sstr << ifs.rdbuf();
    std::string conftext = sstr.str();

    boost::json::error_code ec;
    boost::json::parse_options opt;
    opt.allow_comments = true;

    auto jv = boost::json::parse(conftext, ec,
            boost::json::storage_ptr(), opt);

    auto ctxs = jv.get_array();
    for (size_t i = 0; i < ctxs.size(); i++) {
        auto ctx           = ctxs.at(i);

        std::string ttype = ctx.at("type").as_string().data();
        std::string tname = ctx.at("name").as_string().data();
        log_info("--- t:{}, n:{} --- ", ttype, tname);
        auto props = ctx.at("props").as_object();
        for ( auto it = props.begin();
                it != props.end(); it++) {
            auto k = it->key();
            auto v = it->value();
            log_info("{} -> {}", std::string(k.data(), k.size()),
                    v.as_string().data());
        }
    }
}


int main(int argc, char **argv) {
    tusion::util::init_module_console_log();
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
