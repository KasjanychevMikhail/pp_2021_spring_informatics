// Copyright 2021 Kasjanychev Mikhail
#include <gtest/gtest.h>
#include "./conjugate_gradient.h"

TEST(Conjugate_gradient, ScalarProduct) {
    std::vector<double> v1;
    std::vector<double> v2;

    v1.push_back(3);
    v1.push_back(1);
    v2.push_back(2);
    v2.push_back(5);

    double res = 11;
    ASSERT_EQ(res, scalarProduct(v1, v2));
}
TEST(Conjugate_gradient, MultiMtxVec) {
    std::vector<std::vector<double> > mtx(3);
    std::vector<double> v;

    mtx[0].push_back(3);
    mtx[0].push_back(2);
    mtx[0].push_back(5);
    mtx[1].push_back(0);
    mtx[1].push_back(1);
    mtx[1].push_back(4);
    mtx[2].push_back(3);
    mtx[2].push_back(6);
    mtx[2].push_back(1);

    v.push_back(2);
    v.push_back(4);
    v.push_back(1);

    std::vector<double> res;
    res.push_back(19);
    res.push_back(8);
    res.push_back(31);
    std::vector<double> calc = multiMtxVecSeq(mtx, v);
    for (size_t i = 0; i < res.size(); ++i) {
        ASSERT_EQ(res[i], calc[i]);
    }
}
TEST(Conjugate_gradient, MTX10x10) {
    int n = 10;
    std::vector<std::vector<double> > A = getRandomMatrix(n);
    std::vector<double> vec = getRandomVector(n);
    std::vector<double> res;

    res = calculateStandardRes(A, vec);
    std::vector<double> calc = calculateResPar(A, vec);
    for (size_t i = 0; i < res.size(); ++i) {
        ASSERT_NEAR(calc[i], res[i], 0.05);
    }
}
TEST(Conjugate_gradient, MTX50x50) {
    int n = 50;
    std::vector<std::vector<double> > A = getRandomMatrix(n);
    std::vector<double> vec = getRandomVector(n);
    std::vector<double> res;

    res = calculateStandardRes(A, vec);
    std::vector<double> calc = calculateResPar(A, vec);
    for (size_t i = 0; i < res.size(); ++i) {
        ASSERT_NEAR(calc[i], res[i], 0.05);
    }
}
TEST(Conjugate_gradient, MTX100x100) {
    int n = 100;
    std::vector<std::vector<double> > A = getRandomMatrix(n);
    std::vector<double> vec = getRandomVector(n);
    std::vector<double> res;

    std::vector<double> calcSeq = calculateResSeq(A, vec);

    std::vector<double> calc = calculateResPar(A, vec);

    res = calculateStandardRes(A, vec);
    for (size_t i = 0; i < res.size(); ++i) {
        ASSERT_NEAR(calc[i], res[i], 0.05);
    }
}
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
