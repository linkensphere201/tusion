#include <atomic>
#include <chrono>
#include <thread>

#include <nebula/client/Config.h>
#include <nebula/client/ConnectionPool.h>
#include <nebula/client/Init.h>

#include <gtest/gtest.h>
#include <type_traits>

#include "log.h"
#include "bench_context.h"


TEST(Benchmarker, init) {
    char  arg0[] = "programName";
    char  arg1[] = "--cli_addr=0.0.0.0:1991";
    char  arg2[] = "--concurrency=3";
    char  arg3[] = "--pipeline_depth=5";
    char* argv[] = { &arg0[0], &arg1[0], &arg2[0], &arg3[0], NULL };
    int   argc   = (int)(sizeof(argv) / sizeof(argv[0])) - 1;

    log_info("is movable: {}", std::is_move_constructible<nebula::ConnectionPool>::value);

    auto bm = tusion::bench::Benchmarker::NewBenchmarker();
    bm->init(argc, argv);
}


TEST(Benchmarker, simple) {
    auto bm = tusion::bench::Benchmarker::NewBenchmarker();
    char  arg0[] = "programName";
    char  arg1[] = "--cli_addr=0.0.0.0:1991";
    char  arg2[] = "--concurrency=3";
    char  arg3[] = "--pipeline_depth=5";
    char* argv[] = { &arg0[0], &arg1[0], &arg2[0], &arg3[0], NULL };
    int   argc   = (int)(sizeof(argv) / sizeof(argv[0])) - 1;

    bm->init(argc, argv);
    auto func = [] (int id, tusion::bench::Benchmarker::bptr_t bptr) {
        log_info("I'm #{} benchmarker...", id);
    };
    bm->set(func);
    bm->run();
}

TEST(GoGenerator, init) {
    tusion::bench::SimpleGoGenerator fg("./person.csv", "|");
    fg.getSourceWords(10);
    int c = 20;
    std::string s;
    while (c > 0) {
        fg.getSentence(s);
        log_info("{}", s);
        c--;
    }
}

int
main(int argc, char *argv[]) {
    tusion::util::init_module_console_log();
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
