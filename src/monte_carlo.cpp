#include "monte_carlo.hpp"
#include "cholesky.hpp"

#include <random>
#include <cmath>
#include <algorithm>
#include <stdexcept>

MonteCarloEngine::MonteCarloEngine(
        const MarketSnapshot& snap,
        const Portfolio& pf,
        int horizon)
    : snapshot(snap), portfolio(pf), horizon_days(horizon)
{
    build_cholesky();
}

void MonteCarloEngine::build_cholesky() {
    L = cholesky(snapshot.corr);
}

// Generate one P&L scenario
double MonteCarloEngine::simulate_once(std::mt19937_64& rng) {
    int n = snapshot.tickers.size();

    std::normal_distribution<double> norm(0.0, 1.0);
    std::vector<double> z(n);
    std::vector<double> shock(n);

    // independent shocks
    for (int i = 0; i < n; i++) {
        z[i] = norm(rng);
    }

    // correlated shocks: shock = L * z
    for (int i = 0; i < n; i++) {
        double sum = 0.0;
        for (int k = 0; k <= i; k++)
            sum += L[i][k] * z[k];
        shock[i] = sum;
    }

    // compute tomorrow prices (for horizon_days)
    double dt = horizon_days / 252.0;

    std::unordered_map<std::string, double> new_price;

    for (int i = 0; i < n; i++) {
        const std::string& t = snapshot.tickers[i];
        double S0 = snapshot.spot.at(t);
        double mu = snapshot.mu[i];
        double vol = snapshot.sigma[i];

        double ST = S0 * std::exp(
            (mu - 0.5 * vol * vol) * dt +
            vol * std::sqrt(dt) * shock[i]
        );

        new_price[t] = ST;
    }

    // compute portfolio P&L
    double V0 = 0.0, V1 = 0.0;

    for (const auto& inst : portfolio.instruments) {
        double S0 = snapshot.spot.at(inst.ticker);
        double S1 = new_price.at(inst.ticker);

        if (inst.type == InstrumentType::STOCK) {
            V0 += inst.quantity * S0;
            V1 += inst.quantity * S1;
        } else {
            bool call = (inst.option_type == "CALL");
            double payoff0 = call ?
                std::max(S0 - inst.strike, 0.0) :
                std::max(inst.strike - S0, 0.0);
            double payoff1 = call ?
                std::max(S1 - inst.strike, 0.0) :
                std::max(inst.strike - S1, 0.0);

            V0 += inst.quantity * payoff0;
            V1 += inst.quantity * payoff1;
        }
    }

    return V1 - V0;
}

void MonteCarloEngine::compute(
        int scenarios, double confidence,
        double& var_out, double& es_out)
{
    std::mt19937_64 rng(42);
    std::vector<double> pnl(scenarios);

    for (int s = 0; s < scenarios; s++) {
        pnl[s] = simulate_once(rng);
    }

    std::sort(pnl.begin(), pnl.end());

    int idx = (int)((1.0 - confidence) * scenarios);
    if (idx < 0) idx = 0;
    if (idx >= scenarios) idx = scenarios - 1;

    var_out = -pnl[idx];

    double sum = 0.0;
    for (int i = 0; i <= idx; i++) sum += pnl[i];
    es_out = -(sum / (idx + 1));
}
