#pragma once
#include <vector>
/**
 * @brief Computes the Cholesky decomposition of a symmetric
 *        positive-definite matrix.
 *
 * This function constructs the lower-triangular matrix L such that:
 *      A = L * Lᵀ
 *
 * It is used in the Monte-Carlo engine to introduce correlations
 * between asset returns by transforming independent normal shocks
 * into correlated ones.
 *
 * @param A  Input square matrix (NxN), must be symmetric and
 *           positive-definite.
 *
 * @return Lower-triangular matrix L of the same size, where L * Lᵀ = A.
 *
 * @throws std::runtime_error If the matrix is not positive-definite
 *         (i.e., diagonal element becomes ≤ 0 during factorization).
 */
std::vector<std::vector<double>> cholesky(const std::vector<std::vector<double>>& A);
