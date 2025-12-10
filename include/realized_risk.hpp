#pragma once
#include <string>

/**
 * @brief Container holding historical VaR and ES values.
 *
 * Values are expressed as positive losses (e.g. VaR = 0.03 = 3% loss).
 */
struct RealizedRisk {
    double historical_var;
    double historical_es;
};

class Portfolio;
class MarketDataHistory;


/**
 * @brief Computes realized (historical) VaR and ES.
 *
 * Method:
 *   1. Computes portfolio value on snapshot_date.
 *   2. Collects actual market prices for the next horizon_days.
 *   3. Computes percentage returns.
 *   4. Converts them to losses.
 *   5. Computes:
 *        VaR = 95th percentile of losses,
 *        ES  = mean of worst 5% losses.
 *
 * VaR/ES are capped at zero (cannot be negative).
 *
 * @param portfolio The portfolio whose risk is measured.
 * @param history Historical price database.
 * @param snapshot_date Starting date of VaR horizon.
 * @param horizon_days Number of future trading days to examine.
 *
 * @return RealizedRisk struct with VaR and ES.
 *
 * @throws std::runtime_error if insufficient data is available.
 */
RealizedRisk compute_realized_risk(
    const Portfolio& portfolio,
    const MarketDataHistory& history,
    const std::string& snapshot_date,
    int horizon_days
);
