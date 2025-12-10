#include <iostream>
#include <unordered_set>

#include "market_data_history.hpp"
#include "market_snapshot.hpp"
#include "portfolio.hpp"
#include "monte_carlo.hpp"
#include "realized_risk.hpp"
#include "djia_builder.hpp"
#include "trading_day_utils.hpp"

std::vector<std::string> get_unique_tickers(const Portfolio& p) {
    std::unordered_set<std::string> s;
    std::vector<std::string> out;
    for (auto& inst : p.instruments) {
        if (!s.count(inst.ticker)) {
            s.insert(inst.ticker);
            out.push_back(inst.ticker);
        }
    }
    return out;
}

void remove_missing_tickers(Portfolio& p, const MarketDataHistory& h) {
    std::vector<Instrument> cleaned;
    for (auto& inst : p.instruments) {
        if (h.prices.count(inst.ticker))
            cleaned.push_back(inst);
        else
            std::cout << "WARNING: Ticker " << inst.ticker 
                      << " missing in history => removed\n";
    }
    p.instruments = cleaned;
}

int main(int argc, char** argv){

    bool use_djia = false;
    std::string snapshot_date = "2025-08-08";
    std::string portfolio_path = "data/portfolio_djia.csv";
    std::string history_path   = "data/history/djia";

    double portfolio_value = 10'000'000;
    int lookback_days = 252;
    int horizon_days = 10;
    int scenarios = 10000;
    double confidence = 0.95;

    // === CLI ===
    for (int i = 1; i < argc; i++) {
        std::string a = argv[i];

        if (a == "--djia") { use_djia = true; snapshot_date = argv[++i]; }
        else if (a == "--portfolio") portfolio_path = argv[++i];
        else if (a == "--snapshot") snapshot_date = argv[++i];
        else if (a == "--history") history_path = argv[++i];
        else if (a == "--value") portfolio_value = std::stod(argv[++i]);
        else if (a == "--lookback") lookback_days = std::stoi(argv[++i]);
        else if (a == "--horizon") horizon_days = std::stoi(argv[++i]);
        else if (a == "--scenarios") scenarios = std::stoi(argv[++i]);
        else if (a == "--confidence") confidence = std::stod(argv[++i]);
        else if (a == "--out") portfolio_path = argv[++i];
    }

    // === Load history BEFORE DJIA generation ===
    MarketDataHistory history;
    history.load_directory(history_path);

    auto all_dates = history.get_all_dates();

    // === Align snapshot date to available trading day ===
    snapshot_date = find_common_previous_date(all_dates, snapshot_date);

    if (use_djia) {
        std::cout << "=== GENERATING DJIA PORTFOLIO FOR " << snapshot_date << " ===\n";

        DjiaPortfolioSpec spec;
        spec.snapshot_date = snapshot_date;
        spec.portfolio_value = portfolio_value;
        spec.history_dir = history_path;
        spec.output_file = portfolio_path;

        if (!build_djia_portfolio(spec)) {
            std::cout << "DJIA portfolio generation failed.\n";
            return 1;
        }

        std::cout << "Portfolio saved: " << portfolio_path << "\n\n";
    }

    // === Load portfolio ===
    Portfolio portfolio;
    portfolio.load(portfolio_path);

    remove_missing_tickers(portfolio, history);

    auto tickers = get_unique_tickers(portfolio);

    // === Build snapshot (already aligned) ===
    MarketSnapshot snap = build_snapshot(history, tickers, snapshot_date, lookback_days);

    // Compute initial portfolio value
    double V0 = 0.0;
    for (auto& inst : portfolio.instruments)
        V0 += inst.quantity * snap.spot.at(inst.ticker);

    // === Monte Carlo ===
    MonteCarloEngine mc(snap, portfolio, horizon_days);
    double var_mc, es_mc;
    mc.compute(scenarios, confidence, var_mc, es_mc);

    std::cout << "=== MONTE CARLO RISK ===\n";
    std::cout << "VaR abs = " << var_mc << "\n";
    std::cout << "VaR rel = " << var_mc / V0 << "\n";
    std::cout << "ES  abs = " << es_mc << "\n";
    std::cout << "ES  rel = " << es_mc / V0 << "\n\n";

    // === Realized risk (use SAME aligned date) ===
    RealizedRisk rr = compute_realized_risk(portfolio, history, snapshot_date, horizon_days);

    std::cout << "=== REALIZED (HISTORICAL) RISK ===\n";
    std::cout << "VaR abs = " << rr.historical_var * V0 << "\n";
    std::cout << "VaR rel = " << rr.historical_var << "\n";
    std::cout << "ES  abs = " << rr.historical_es * V0 << "\n";
    std::cout << "ES  rel = " << rr.historical_es << "\n";

    return 0;
}
