#include "bench_context.h"
#include "log.h"

#include <boost/algorithm/string.hpp>

#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>

namespace tusion {
namespace bench {
void SimpleGoGenerator::getSourceWords(int count, bool shuffle) {
    words_.reserve(count * 2);
    std::ifstream f1(f1_);
    std::string   line;
    while (std::getline(f1, line) && count > 0) {
        std::vector<std::string> strs;
        boost::split(strs, line, boost::is_any_of(delim_));
        words_.emplace_back(strs[0]);
        count--;
    }
    if (shuffle) {
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::shuffle (words_.begin(), words_.end(), std::default_random_engine(seed));
    }
}
} // namespace bench
} // namespace tusion
