#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/extended_p_square.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/moment.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/array.hpp>

#include <gtest/gtest.h>
#include <iostream>
#include <ostream>

#include "log.h"
#include "bench_context.h"

TEST(x, basic) {
    tusion::bench::Acc<double> ta;

    ta.AddMetric(10.0);
    ta.AddMetric(10.0);
    ta.AddMetric(10.0);
    ta.AddMetric(10.0);
    ta.AddMetric(10.0);
    ta.AddMetric(10.0);
    ta.AddMetric(10.0);
    ta.AddMetric(15.0);
    ta.AddMetric(15.0);
    ta.AddMetric(15.0);
    ta.AddMetric(15.0);
    ta.AddMetric(15.0);
    ta.AddMetric(20.0);
    ta.AddMetric(20.0);
    ta.AddMetric(20.0);
    ta.AddMetric(20.0);
    ta.AddMetric(20.0);

    /*
    ta.AddMetric(1.0);
    ta.AddMetric(1.0);
    ta.AddMetric(1.0);
    ta.AddMetric(1.0);
    ta.AddMetric(1.0);
    ta.AddMetric(1.0);
    ta.AddMetric(1.0);
    ta.AddMetric(1.0);
    ta.AddMetric(1.0);
    ta.AddMetric(1.0);
    ta.AddMetric(1.0);
    ta.AddMetric(1.5);
    ta.AddMetric(1.5);
    ta.AddMetric(1.5);
    ta.AddMetric(2.0);
    */

    std::cout << "metric count: " << ta.MetricCount() << std::endl;
    std::cout << "mean: " << ta.Mean() << std::endl;
    std::cout << "min : " << ta.Min() << std::endl;
    std::cout << "max : " << ta.Max() << std::endl;
    std::cout << "P90 : " << ta.P90() << std::endl;
    std::cout << "P99 : " << ta.P99() << std::endl;
    std::cout << "P100 : " << ta.P100() << std::endl;
}


using namespace boost::accumulators;

TEST(acc, basic) {
    boost::array<double, 5> probs = {0.5, 0.75, 0.9, 0.99, 1};
    accumulator_set<
        double, stats<tag::mean, tag::max, tag::min, tag::extended_p_square>>
        acc(tag::extended_p_square::probabilities = probs);

    acc(1.0);
    acc(1.0);
    acc(1.0);
    acc(1.0);
    acc(1.0);
    acc(1.0);
    acc(1.0);
    acc(1.0);
    acc(1.0);
    acc(1.0);
    acc(1.0);
    acc(1.5);
    acc(1.5);
    acc(1.5);
    acc(2.0);

    std::cout << "mean: " << mean(acc) << std::endl;
    std::cout << "min: " << min(acc) << std::endl;
    std::cout << "max: " << max(acc) << std::endl;
    std::cout << "p50: " << extended_p_square(acc)[0] << std::endl;
    std::cout << "p75: " << extended_p_square(acc)[1] << std::endl;
    std::cout << "p90: " << extended_p_square(acc)[2] << std::endl;
    std::cout << "p99: " << extended_p_square(acc)[3] << std::endl;
    std::cout << "p100: " << extended_p_square(acc)[4] << std::endl;
}


int main(int argc, char **argv) {
    tusion::util::init_module_console_log();
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
