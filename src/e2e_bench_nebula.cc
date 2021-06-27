#include "bench_context.h"
#include "log.h"
#include "util.h"

#include <chrono>
#include <gflags/gflags.h>
#include <folly/ScopeGuard.h>
#include <folly/chrono/Hardware.h>
#include <memory>
#include <thread>


void run_NebulaBenchmark(int argc, char *argv[]) {
    auto bm = tusion::bench::Benchmarker::NewBenchmarker();
    bm->init(argc, argv);

    auto benchrunner = [](int id, tusion::bench::Benchmarker::bptr_t bptr) {
        std::string sv;
        auto        ctx     = bptr->getContext(id);
        auto        session = ctx->pool_.getSession("root", "p1");
        if (!session.valid()) {
            log_error("t# {} - get session failed...", id);
            return;
        }
        auto result = session.execute("use ldbct1;");
        if (result.errorCode != nebula::ErrorCode::SUCCEEDED) {
            log_error("t# {} - use space failed: {}...",  id, result.errorCode);
            return;
        }
        log_info("t# {} - ready to lauch test ...", id);

        auto consumer = [&id, &ctx, &session, &bptr] () {
            std::string s;
            std::atomic<int> count_flying_request = 0;
            SCOPE_EXIT {
                while (ctx->mpmc_.read(s)) {
                    // noop
                }
                while (count_flying_request.load() > 0) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
                log_info("t#{} consumer stop...", id);
            };
            while (true) {
                while (!ctx->mpmc_.read(s)) {
                    if (ctx->should_stop_.load()) {
                        return;
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
                {
                    while (count_flying_request.load() > bptr->pipeline_depth_) {
                        // block by sleep for simplicity
                        // log_info("t#{} wait on-the-fly req is {}...", id, count_flying_request.load());
                        std::this_thread::sleep_for(std::chrono::microseconds(20));
                    }

                    count_flying_request.fetch_add(1);

                    session.asyncExecute(
                            s,
                            [&count_flying_request, &ctx, &s] (nebula::ExecutionResponse&& cbResult) {
                                count_flying_request.fetch_sub(1);
                                if (cbResult.errorCode != nebula::ErrorCode::SUCCEEDED) {
                                    ctx->acc_.AddErrorMetric();
                                    return;
                                }
                                // log_info("request is completed, spent {} us", cbResult.latencyInUs);
                                // log_info("GQL - {} \nres-\n {}", s, cbResult.data->toString());
                                ctx->acc_.AddMetric(cbResult.latencyInUs);
                            });
                }
            }
        };

        auto producer = [&id, &ctx] () {
            std::string smtm;
            int ct = 0;
            SCOPE_EXIT {
                log_info("t#{} producer stop...", id);
            };

            while(true) {
                ctx->sgg_->getSentence(smtm);
                while (! ctx->mpmc_.write(std::move(smtm))) {
                    if (ctx->should_stop_.load()){
                        return;
                    }
                }
                if (ct % 100 == 0 && ctx->should_stop_.load()) {
                    return;
                }
                ct++;
            }
        };

        std::thread p(producer);
        std::thread c(consumer);

        p.join();
        c.join();

        std::string sum;
        ctx->summary(sum);
        log_info("---t# {} summaries: {}", id, sum);
    };

    bm->set(benchrunner);
    bm->run();
}

int main(int argc, char *argv[]) {
    tusion::util::init_module_console_log();
    run_NebulaBenchmark(argc, argv);
    return 0;
}
