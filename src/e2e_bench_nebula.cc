#include "bench_context.h"
#include "log.h"
#include <chrono>
#include <gflags/gflags.h>
#include <thread>

void run_NebulaBenchmark(int argc, char *argv[]) {
    auto bm = tusion::bench::Benchmarker::NewBenchmarker();
    bm->init(argc, argv);

    auto benchrunner = [](int id, tusion::bench::Benchmarker::bptr_t bptr) {
        std::string sv;
        int         cx = 0;
        while (true) {
            auto ctx = bptr->getContext(id);
            ctx->sgg_->getSentence(sv);
            if (cx % 10 == 0) {
                log_info("t#{} go GQL - {}", id, sv);
                if (ctx->should_stop_.load()) {
                    break;
                }
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
            cx++;
        }
    };

    bm->set(benchrunner);
    bm->run();
}

int main(int argc, char *argv[]) {
    tusion::util::init_module_console_log();
    run_NebulaBenchmark(argc, argv);

    return 0;
}
