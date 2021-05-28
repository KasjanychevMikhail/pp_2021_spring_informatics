// Copyright 2021 Gushchin Artem
#include <gtest/gtest.h>

#include <cmath>
#include <iostream>
#include <vector>

#include "../../../3rdparty/unapproved/unapproved.h"

#include "./simpsonMethod.h"

TEST(SimpsonsMethodIntegrating, simplestConstFunction) {
    auto func = [](const std::vector<double>& arguments) { return 8; };
    std::vector<std::pair<double, double>> segments = { {-3, 1} };

    auto seqResult = simpsonsMethod(func, segments, 10);
    auto parallelResult = parallelSimpsonsMethod(func, segments, 10);

    ASSERT_NEAR(32, parallelResult, 0.00001);
    ASSERT_NEAR(seqResult, parallelResult, 0.00001);
}

TEST(SimpsonsMethodIntegrating, oneDimentionalFunction) {
    auto func = [](const std::vector<double>& arguments) {
        return pow(arguments[0], 3) + cos(arguments[0]);
    };
    std::vector<std::pair<double, double>> segments = { {-5, 4} };

    auto seqResult = simpsonsMethod(func, segments, 100);
    auto parallelResult = parallelSimpsonsMethod(func, segments, 100);

    ASSERT_NEAR(-93.96572, parallelResult, 0.00001);
    ASSERT_NEAR(seqResult, parallelResult, 0.00001);
}

TEST(SimpsonsMethodIntegrating, twoDimentionalFunction) {
    auto func = [](const std::vector<double>& arguments) {
        return pow(arguments[0], 2) + pow(arguments[1], 2);
    };
    std::vector<std::pair<double, double>> segments = { {-1, 3}, {7, 9} };

    auto seqResult = simpsonsMethod(func, segments, 100);
    auto parallelResult = parallelSimpsonsMethod(func, segments, 100);

    ASSERT_NEAR(533.33333, parallelResult, 0.00001);
    ASSERT_NEAR(seqResult, parallelResult, 0.00001);
}

TEST(SimpsonsMethodIntegrating, loadTest) {
    auto func = [](const std::vector<double>& arguments) {
        return arguments[0] + arguments[1] + cos(arguments[2]);
    };
    std::vector<std::pair<double, double>> segments = { { 0, 5 }, { -2, 3 },
                                                        { -1, 3 } };

    auto seq_start = std::chrono::steady_clock::now();
    auto seq = simpsonsMethod(func, segments, 100000000);
    auto seq_end = std::chrono::steady_clock::now();

    auto par_start = std::chrono::steady_clock::now();
    auto par = parallelSimpsonsMethod(func, segments, 100000000);
    auto par_end = std::chrono::steady_clock::now();

    std::cout << "Seq time: "
              << std::chrono::duration<double>(seq_end - seq_start).count()
              << std::endl;
    std::cout << "Par time: "
              << std::chrono::duration<double>(par_end - par_start).count()
              << std::endl;

    ASSERT_NEAR(324.56477, par, 0.00001);
    ASSERT_NEAR(seq, par, 0.00001);
}

TEST(SimpsonsMethodIntegrating, throwsWhenSegmentsAreEmpty) {
    auto func = [](const std::vector<double>& arguments) {
        return arguments[0] + arguments[1] + cos(arguments[2]);
    };
    std::vector<std::pair<double, double>> segments;
    ASSERT_ANY_THROW(parallelSimpsonsMethod(func, segments, 100));
}

TEST(SimpsonsMethodIntegrating, throwsWhenStepsCountIsZero) {
    auto func = [](const std::vector<double>& arguments) {
        return arguments[0] + arguments[1] + cos(arguments[2]);
    };
    std::vector<std::pair<double, double>> segments = { { 0, 5 },
                                                        { -2, 3 },
                                                        { -1, 3 } };
    ASSERT_ANY_THROW(parallelSimpsonsMethod(func, segments, 0));
}