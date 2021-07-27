#include "ContentGenerators.h"

namespace tusion {
namespace gen {

const char *kPNumber = "[0-9]+";
const char *kPRange  = "range\\[[0-9]+,[0-9]+\\]";
const char *kPString = "string\\([0-9]+\\)";
const char *kPBool   = "bool";
const char *kPInt    = "int";
const char *kPDate   = "date";
const char *kPEnum   = "E\\{[a-zA-Z]+\\}";

void GetNumbers(const std::string &str, std::vector<size_t> &nbs) {
    boost::cregex_iterator endIt;
    for (boost::cregex_iterator it(str.data(), str.data() + str.size(),
                                   boost::regex(kPNumber));
         it != endIt; it++) {
        size_t v = boost::lexical_cast<size_t>(
            std::string((*it)[0].first, (*it)[0].second - (*it)[0].first)
                .c_str());
        nbs.emplace_back(v);
    }
}

void GetEnumStrings(const std::string &str, std::vector<std::string> &estrs) {
    boost::cregex_iterator endIt;
    for (boost::cregex_iterator it(str.data(), str.data() + str.size(),
                                   boost::regex(kPEnum));
         it != endIt; it++) {
        estrs.emplace_back((*it)[0].first + 2,
                           (*it)[0].second - (*it)[0].first - 3);
    }
}

bool GetRange(const std::string &str, std::shared_ptr<GeneratorRange> &m) {
    if (boost::regex_match(str, boost::regex(kPRange))) {
        std::vector<size_t> vec;
        GetNumbers(str, vec);
        if (vec.size() >= 2) {
            m.reset(new GeneratorRange(vec[0], vec[1]));
            return true;
        }
    }
    return false;
}

bool GetInt(const std::string &str, std::shared_ptr<GeneratorRange> &m,
            size_t min_, size_t max_) {
    if (boost::regex_match(str, boost::regex(kPInt))) {
        m.reset(new GeneratorRange(min_, max_));
        return true;
    }
    return false;
}

bool GetString(const std::string &str, std::shared_ptr<GeneratorString> &m) {
    if (boost::regex_match(str, boost::regex(kPString))) {
        std::vector<size_t> vec;
        GetNumbers(str, vec);
        if (vec.size() > 0) {
            m.reset(new GeneratorString(vec[0]));
            return true;
        }
    }
    return false;
}

bool GetBool(const std::string &str, std::shared_ptr<GeneratorBool> &m) {
    if (str.compare(kPBool) == 0) {
        m.reset(new GeneratorBool);
        return true;
    }
    return false;
}

bool GetEnum(const std::string &str, std::shared_ptr<GeneratorEnum> &m) {
    std::vector<std::string> enumstrs;
    GetEnumStrings(str, enumstrs);
    m.reset(new GeneratorEnum(std::move(enumstrs)));
    return m->enums.size() > 0;
}

bool GetDate(const std::string &str, std::shared_ptr<GeneratorDate> &m, size_t freq) {
    if (str.compare(kPDate) == 0) {
        m.reset(new GeneratorDate(freq));
        return true;
    }
    return false;
}

std::shared_ptr<ContentGenerator> GetContentGenerator(const std::string &str) {
    switch (str[0]) {
    case 'r': {
        std::shared_ptr<GeneratorRange> m;
        if (GetRange(str, m)) {
            return m;
        }
    } break;
    case 's': {
        std::shared_ptr<GeneratorString> m;
        if (GetString(str, m)) {
            return m;
        }
    } break;
    case 'b': {
        std::shared_ptr<GeneratorBool> m;
        if (GetBool(str, m)) {
            return m;
        }
    } break;
    case 'i': {
        std::shared_ptr<GeneratorRange> m;
        if (GetInt(str, m)) {
            return m;
        }
    } break;
    case 'E': {
        std::shared_ptr<GeneratorEnum> m;
        if (GetEnum(str, m)) {
            return m;
        }
    } break;
    case 'd': {
        std::shared_ptr<GeneratorDate> m;
        if (GetDate(str, m)) {
            return m;
        }
    }
    default:;
    };
    log_info("failed to parse pattern: {} ...", str);
    return std::shared_ptr<ContentGenerator>();
}

} // namespace gen
} // namespace tusion
