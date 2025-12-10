#include <gtest/gtest.h>
#include "cholesky.hpp"

TEST(CholeskyTest, IdentityMatrix) {
    std::vector<std::vector<double>> A = {
        {1, 0},
        {0, 1}
    };

    auto L = cholesky(A);

    EXPECT_NEAR(L[0][0], 1.0, 1e-12);
    EXPECT_NEAR(L[1][1], 1.0, 1e-12);
    EXPECT_NEAR(L[1][0], 0.0, 1e-12);
}
