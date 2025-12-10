#include "realized_risk.hpp"
#include "market_data_history.hpp"
#include "portfolio.hpp"
#include <algorithm>
#include <stdexcept>

RealizedRisk compute_realized_risk(
    const Portfolio& portfolio,
    const MarketDataHistory& history,
    const std::string& snapshot_date,
    int horizon_days
) {
    // 1. Compute portfolio value on snapshot date
    double V0 = 0.0;
    for (const auto& pos : portfolio.instruments) {
        double p = history.get_price(pos.ticker, snapshot_date);
        V0 += pos.quantity * p;
    }

    // 2. Collect future dates
    auto future_dates = history.get_future_dates(snapshot_date, horizon_days);
    if (future_dates.empty()) {
        throw std::runtime_error("Not enough future dates for realized VaR");
    }

    // 3. Compute losses, not returns!
    std::vector<double> losses;
    losses.reserve(future_dates.size());

    for (const auto& d : future_dates) {
        double Vt = 0.0;
        for (const auto& pos : portfolio.instruments) {
            double p = history.get_price(pos.ticker, d);
            Vt += pos.quantity * p;
        }

        double ret = (Vt - V0) / V0;
        double loss = -ret;  // <-- ключевой момент
        losses.push_back(loss);
    }

    // 4. Sort losses ascending (worst = biggest positive)
    std::sort(losses.begin(), losses.end());  

    int n = losses.size();
    int idx = int(0.95 * n);
    if (idx >= n) idx = n - 1;

    double VaR = losses[idx];

    // 5. ES = average of worst losses >= VaR threshold
    double sum = 0.0;
    int count = 0;

    for (int i = idx; i < n; i++) {
        sum += losses[i];
        count++;
    }

    double ES = (count > 0) ? (sum / count) : 0.0;

    // 6. VaR should never be negative
    if (VaR < 0) VaR = 0;
    if (ES < 0) ES = 0;

    return {VaR, ES};
}
