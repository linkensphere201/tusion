
#include "bench_context.h"
#include "log.h"
#include <gflags/gflags.h>
#include <chrono>
#include <thread>
#include <sstream>

DEFINE_string(cli_addr, "0.0.0.0:9669", "graphd address");
DEFINE_int32(concurrency, 10, "count of clients");
DEFINE_int32(pipeline_depth, 10, "max in flying request count");
DEFINE_int32(max_conn_count, 10, "count of conn each cli");
DEFINE_int32(timeout_ms, 1000, "timeout in milisecond");
DEFINE_int32(preload_count, 30, "pre loaded count");
DEFINE_int32(run_time, 180, "run time");
DEFINE_string(input_source_file, "person.csv", "input file");
DEFINE_string(delim, "|", "input file 's delimters");

namespace tusion {
namespace bench {

bool Context::init(const std::string & input, const std::string &delim, int pre_load_count) {
    pool_.init({addr_}, nebula::Config{timeoutms_, 0 /*idle time*/, maxconn_,
                                       0 /*min conn*/});
    sgg_.reset(new SimpleGoGenerator(input, delim));
    sgg_->getSourceWords(pre_load_count, true);
    return true;
}

void Context::summary(std::string & result) {
    std::stringstream ss;
    ss << "Request Total/Error: ";
    ss << acc_.MetricCount() << "/";
    ss << acc_.ErrorCount() << ". Resp ";
    ss << " Mean/Min/Max/P50/P99: ";
    ss << ((double)acc_.Mean()/1000) << " /";
    ss << ((double)acc_.Min()/1000) << " /";
    ss << ((double)acc_.Max()/1000) << " /";
    ss << ((double)acc_.P50()/1000) << " /";
    ss << ((double)acc_.P99()/1000);
    result = ss.str();
}

void Benchmarker::init(int argc, char *argv[]) {
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    nebula::init(&argc, &argv);

    log_info("- cli_addr        : {}", FLAGS_cli_addr);
    log_info("- concurrency     : {}", FLAGS_concurrency);
    log_info("- timeout in ms   : {}", FLAGS_timeout_ms);
    log_info("- max_conn_count  : {}", FLAGS_max_conn_count);
    log_info("- pipeline_depth  : {}", FLAGS_pipeline_depth);
    log_info("- pre loaded count: {}", FLAGS_preload_count);
    log_info("- run time        : {}", FLAGS_run_time);
    log_info("- input file      : {}", FLAGS_input_source_file);
    log_info("- delimter        : {}", FLAGS_delim);

    pipeline_depth_ = FLAGS_pipeline_depth;
    ctxs_.reserve(FLAGS_concurrency);

    for (size_t i = 0; i < FLAGS_concurrency; i++) {
        ctxs_.emplace_back(new Context(FLAGS_cli_addr, FLAGS_timeout_ms,
                                       FLAGS_max_conn_count,
                                       pipeline_depth_));
    }
    for (auto & c : ctxs_) {
        c->init(FLAGS_input_source_file, FLAGS_delim, FLAGS_preload_count);
    }
}

void Benchmarker::set(cb_t cb) { func_ = cb; }
void Benchmarker::run() {
    log_info("---- start to run benchmark threads ---");
    std::vector<std::thread> bmthreads;
    for (size_t i = 0; i < FLAGS_concurrency; i++) {
        bmthreads.push_back(std::thread(func_, i, shared_from_this()));
    }

    log_info("---- wait for {} secs.... ---", FLAGS_run_time);
    std::this_thread::sleep_for(std::chrono::seconds(FLAGS_run_time));

    log_info("---- should stop, wait for benchmark completed ---");
    for (auto &c : ctxs_) {
        c->should_stop_.store(true);
    }
    for (auto &i : bmthreads) {
        i.join();
    }
    log_info("---- benchmark completed ---");
}
Context *Benchmarker::getContext(int idx) {
    return idx < ctxs_.size() ? ctxs_[idx].get() : nullptr;
}

} // namespace bench
} // namespace tusion
