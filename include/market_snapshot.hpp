#pragma once
#include <vector>
#include <string>
#include <unordered_map>

/**
 * @brief Represents a calibrated market state for Monte Carlo simulation.
 *
 * Contains:
 *   - list of required tickers,
 *   - spot prices on the snapshot date,
 *   - daily log-return mean vector (mu),
 *   - daily volatilities (sigma),
 *   - correlation matrix (corr).
 *
 * This structure is generated from historical market data and used
 * as input to Monte Carlo risk calculations.
 */

struct MarketSnapshot {
    std::vector<std::string> tickers;

    // Spot prices at snapshot date
    std::unordered_map<std::string, double> spot;

    // Daily log-return mean and volatility
    std::vector<double> mu;
    std::vector<double> sigma;

    // Correlation matrix
    std::vector<std::vector<double>> corr;
};

// Forward declaration
class MarketDataHistory;


/**
 * @brief Builds a market snapshot calibrated from historical prices.
 *
 * Steps:
 *   1. Reads spot prices for all tickers on snapshot_date.
 *   2. Extracts lookback_days of historical returns.
 *   3. Computes per-asset drift (mu) and volatility (sigma).
 *   4. Computes full correlation matrix.
 *
 * @param hist MarketDataHistory object containing all historical prices.
 * @param tickers List of portfolio tickers.
 * @param snapshot_date Date on which risk should be evaluated.
 * @param lookback_days Number of past trading days for estimation.
 *
 * @throws std::runtime_error if insufficient data is available.
 */
// Build snapshot from historical data
MarketSnapshot build_snapshot(
    const MarketDataHistory& hist,
    const std::vector<std::string>& tickers,
    const std::string& snapshot_date,
    int lookback_days);
