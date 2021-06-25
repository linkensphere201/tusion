#pragma once
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/extended_p_square.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/moment.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/array.hpp>
#include <folly/MPMCQueue.h>

#include <nebula/client/Config.h>
#include <nebula/client/ConnectionPool.h>
#include <nebula/client/Init.h>

#include <functional>
#include <atomic>
#include <memory>

namespace tusion {
namespace bench {

template <typename T> struct Acc {
    using acc_set = boost::accumulators::accumulator_set<
        T, boost::accumulators::stats<
               boost::accumulators::tag::mean, boost::accumulators::tag::max,
               boost::accumulators::tag::min,
               boost::accumulators::tag::extended_p_square>>;
    Acc()
        : acc(boost::accumulators::tag::extended_p_square::probabilities =
                  boost::array<double, 5>{0.5, 0.75, 0.9, 0.99, 1}),
          pt_count(0) {}
    void AddMetric(T m) {
        acc(m);
        pt_count++;
    }
    void AddErrorMetric() {
        err_count++;
    }
    size_t MetricCount() { return pt_count; }
    size_t ErrorCount() { return err_count; }
    T      Mean() { return boost::accumulators::mean(acc); }
    T      Min() { return boost::accumulators::min(acc); }
    T      Max() { return boost::accumulators::max(acc); }
    T      P50() { return boost::accumulators::extended_p_square(acc)[0]; }
    T      P75() { return boost::accumulators::extended_p_square(acc)[1]; }
    T      P90() { return boost::accumulators::extended_p_square(acc)[2]; }
    T      P99() { return boost::accumulators::extended_p_square(acc)[3]; }
    T      P100() { return boost::accumulators::extended_p_square(acc)[4]; }

    acc_set acc;
    size_t  pt_count;
    size_t  err_count;
};

class GQLGenerator;
class SimpleGoGenerator;

struct Context {
    Context(const std::string &addr, int timeoutms, int maxconn, int q_depth)
        : addr_(addr), timeoutms_(timeoutms), maxconn_(maxconn),
          should_stop_(false), mpmc_(q_depth) {}

    bool init(const std::string &input, const std::string &delim,
              int pre_load_count);
    void summary(std::string &result);

    Context(const Context &) = delete;
    std::string       addr_;
    uint32_t          timeoutms_;
    uint32_t          maxconn_;
    std::atomic<bool> should_stop_;

    nebula::ConnectionPool             pool_;
    std::shared_ptr<SimpleGoGenerator> sgg_;
    folly::MPMCQueue<std::string>      mpmc_;
    Acc<uint64_t>                      acc_;
};

struct Benchmarker : std::enable_shared_from_this<Benchmarker> {
    using bptr_t = std::shared_ptr<Benchmarker>;
    using cb_t   = std::function<void(int, bptr_t)>;

    cb_t                                  func_;
    std::vector<std::unique_ptr<Context>> ctxs_;

    uint32_t pipeline_depth_{1};

    Context *getContext(int idx);

    static bptr_t NewBenchmarker() { return std::make_shared<Benchmarker>(); }
    void          init(int argc, char *argv[]);
    void          set(cb_t cb);
    void          run();
};

class GQLGenerator {
  public:
    virtual ~GQLGenerator() {}
    virtual void getSentence(std::string &sentence) = 0;
};

class SimpleGoGenerator : public GQLGenerator {
  public:
    SimpleGoGenerator(const std::string &f1, const std::string &delim)
        : f1_(f1), delim_(delim) {}

    void getSentence(std::string &sentence) final {
        sentence = "GO 2 STEPS FROM ";
        sentence.append(words_[idx_++ % words_.size()]);
        sentence.append(" OVER KNOWS");
    }

    // GO 2 STEP FROM {x} OVER KNOWS
    void                     getSourceWords(int count, bool shuffle = false);
    std::string              f1_;
    std::string              delim_;
    int                      idx_{0};
    std::vector<std::string> words_;
};

} // namespace bench
} // namespace tusion
