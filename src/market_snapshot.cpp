#include "market_snapshot.hpp"
#include "market_data_history.hpp"

#include <cmath>
#include <stdexcept>

MarketSnapshot build_snapshot(
    const MarketDataHistory& hist,
    const std::vector<std::string>& tickers,
    const std::string& snapshot_date,
    int lookback_days)
{
    MarketSnapshot snap;
    snap.tickers = tickers;

    int n = tickers.size();
    if (n == 0)
        throw std::runtime_error("build_snapshot: empty ticker list");

    // ---- 1. Spot prices on snapshot_date ----
    for (const auto& t : tickers) {
        snap.spot[t] = hist.get_price(t, snapshot_date);
    }

    // ---- 2. Obtain lookback dates ----
    auto dates = hist.get_past_dates(snapshot_date, lookback_days);
    if (dates.size() < 2)
        throw std::runtime_error("Not enough history for snapshot");

    std::vector<std::vector<double>> returns(n);

    for (int i = 0; i < n; i++) {
        const std::string& t = tickers[i];

        for (int d = 1; d < (int)dates.size(); d++) {
            double p_prev = hist.get_price(t, dates[d]);
            double p_curr = hist.get_price(t, dates[d - 1]);

            double r = std::log(p_curr / p_prev);
            returns[i].push_back(r);
        }
    }

    // ---- 3. Compute mean μ and volatility σ ----
    snap.mu.resize(n);
    snap.sigma.resize(n);

    for (int i = 0; i < n; i++) {
        double mean = 0.0;
        for (double r : returns[i]) mean += r;
        mean /= returns[i].size();
        snap.mu[i] = mean;

        double var = 0.0;
        for (double r : returns[i]) var += (r - mean) * (r - mean);
        var /= (returns[i].size() - 1);
        snap.sigma[i] = std::sqrt(var);
    }

    // ---- 4. Correlation matrix ----
    snap.corr.assign(n, std::vector<double>(n, 0.0));

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {

            if (i == j) {
                snap.corr[i][j] = 1.0;
                continue;
            }

            double cov = 0.0;
            for (int k = 0; k < (int)returns[i].size(); k++) {
                cov += (returns[i][k] - snap.mu[i]) *
                       (returns[j][k] - snap.mu[j]);
            }

            cov /= (returns[i].size() - 1);

            snap.corr[i][j] = cov / (snap.sigma[i] * snap.sigma[j]);
        }
    }

    return snap;
}
