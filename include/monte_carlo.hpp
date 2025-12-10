#pragma once

#include "market_snapshot.hpp"
#include "portfolio.hpp"
#include "cholesky.hpp"

#include <vector>
#include <string>
#include <random>   

/**
 * @brief Monte Carlo simulation engine for multi-asset portfolios.
 *
 * The engine:
 *   - uses GBM (geometric Brownian motion) price dynamics,
 *   - generates correlated shocks using Cholesky decomposition,
 *   - simulates value changes over a given horizon,
 *   - computes portfolio-level P&L,
 *   - estimates Value-at-Risk (VaR) and Expected Shortfall (ES).
 *
 * Supports stocks and European-style options.
 */

class MonteCarloEngine {
public:

    /**
     * @brief Constructs the Monte Carlo engine.
     *
     * @param snap Market snapshot containing drift, vol, correlation.
     * @param portfolio Portfolio of instruments (stock / option).
     * @param horizon_days Horizon for VaR simulation in trading days.
     *
     * The constructor automatically computes the Cholesky matrix
     * from the correlation matrix.
     */
    MonteCarloEngine(const MarketSnapshot& snap,
                     const Portfolio& portfolio,
                     int horizon_days);
    
    /**
     * @brief Runs Monte Carlo simulation and computes VaR and ES.
     *
     * @param scenarios Number of Monte Carlo paths to generate.
     * @param confidence Confidence level (e.g., 0.95).
     * @param var_out Output absolute VaR value.
     * @param es_out Output absolute ES value.
     *
     * Resulting VaR/ES are positive numbers representing losses.
     */
    void compute(int scenarios, double confidence,
                 double& var_out, double& es_out);

private:
    MarketSnapshot snapshot; ///< calibrated market data
    Portfolio portfolio;  ///< list of instruments
    int horizon_days;  ///< VaR horizon in days

    std::vector<std::vector<double>> L; // Cholesky matrix
    
    /**
     * @brief Builds Cholesky matrix L from correlation matrix.
     *
     * Throws if the correlation matrix is not positive definite.
     */
    void build_cholesky();
    
    /**
     * @brief Simulates a single correlated GBM scenario and computes P&L.
     *
     * @param rng Reference to random number generator.
     *
     * @return Scenario P&L (positive = profit, negative = loss).
     */
    double simulate_once(std::mt19937_64& rng);
};
