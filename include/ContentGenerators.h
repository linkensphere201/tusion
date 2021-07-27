#pragma once
#include "log.h"
#include "util.h"

#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <fmt/core.h>
#include <limits>
#include <memory>
#include <string>
#include <time.h>

namespace tusion {
namespace gen {

extern const char *kPNumber;
extern const char *kPRange;
extern const char *kPString;
extern const char *kPBool;
extern const char *kPInt;
extern const char *kPEnum;
extern const char *kPDate;

struct ContentGenerator {
    virtual ~ContentGenerator() {}
    virtual void serilize(std::string &content) = 0;
};

struct GeneratorRange : public ContentGenerator {
    GeneratorRange(size_t mi, size_t ma) : min(mi), max(ma), uni(min, max) {}
    void         generate(size_t &s) { s = uni.Get(); }
    virtual void serilize(std::string &content) {
        size_t i;
        generate(i);
        content = fmt::format("{}", i);
    }
    size_t                            min;
    size_t                            max;
    tusion::util::UniformDistribution uni;
};

struct GeneratorString : public ContentGenerator {
    int                           size;
    tusion::util::RandomGenerator rnd;
    GeneratorString(int s) : size(s) {}
    void         generate(std::string &s) { rnd.Generate(size, s); }
    virtual void serilize(std::string &content) { generate(content); }
};

struct GeneratorBool : public ContentGenerator {
    GeneratorRange g;
    GeneratorBool() : g(0, 1) {}
    void generate(bool &s) {
        size_t i;
        g.generate(i);
        s = i == 0 ? true : false;
    }
    virtual void serilize(std::string &content) {
        bool i;
        generate(i);
        content = i ? "True" : "False";
    }
};

struct GeneratorEnum : public ContentGenerator {
    GeneratorRange           g;
    std::vector<std::string> enums;
    GeneratorEnum(std::vector<std::string> &&enums_)
        : g(0, enums_.size() - 1), enums(enums_) {}
    void generate(std::string &s) {
        size_t idx;
        g.generate(idx);
        s = enums[idx];
    }
    virtual void serilize(std::string &content) { generate(content); }
};

struct GeneratorDate : public ContentGenerator {
    GeneratorRange g;
    std::string    default_date;
    size_t gcount;
    size_t freq;

    GeneratorDate(size_t frequancy)
        : g(1500927601UL, 1627219067UL),
        default_date("2018-05-23T18:23:00"),
        gcount(0),
        freq(frequancy) {}

    void UpdateDate() {
        size_t ts;
        g.generate(ts);
        std::tm tmm;
        if (nullptr != gmtime_r((const time_t *)&ts, &tmm)) {
            default_date = fmt::format("{}-{:02}-{:02}T{:02}:{:02}:{:02}",
                    (tmm.tm_year + 1900),
                    (tmm.tm_mon + 1),
                    tmm.tm_mday,
                    tmm.tm_hour,
                    tmm.tm_min,
                    tmm.tm_sec);
        }
    }

    virtual void serilize(std::string &content) {
        content = default_date;
        gcount++;
        if (gcount % freq == 0) {
            UpdateDate();
        }
    }
};

void GetNumbers(const std::string &str, std::vector<size_t> &nbs);
void GetEnumStrings(const std::string &str, std::vector<std::string> &estrs);
bool GetRange(const std::string &str, std::shared_ptr<GeneratorRange> &m);
bool GetInt(const std::string &str, std::shared_ptr<GeneratorRange> &m,
            size_t min_ = 100000000UL,
            size_t max_ = std::numeric_limits<size_t>::max());
bool GetString(const std::string &str, std::shared_ptr<GeneratorString> &m);
bool GetBool(const std::string &str, std::shared_ptr<GeneratorBool> &m);
bool GetEnum(const std::string &str, std::shared_ptr<GeneratorEnum> &m);
bool GetDate(const std::string &str, std::shared_ptr<GeneratorDate> &m, size_t freq = 10);

std::shared_ptr<ContentGenerator> GetContentGenerator(const std::string &str);

} // namespace gen
} // namespace tusion
