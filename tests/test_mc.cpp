#include <gtest/gtest.h>
#include "monte_carlo.hpp"
#include "market_snapshot.hpp"
#include "portfolio.hpp"

TEST(MonteCarloTest, BasicSanity) {
    MarketSnapshot snap;
    snap.tickers = {"AAPL"};
    snap.spot["AAPL"] = 100.0;
    snap.mu = {0.0};
    snap.sigma = {0.2};
    snap.corr = {{1.0}};

    Portfolio p;
    p.instruments.push_back({InstrumentType::STOCK, "AAPL", 10});

    MonteCarloEngine mc(snap, p, 10);

    double var, es;
    mc.compute(5000, 0.95, var, es);

    EXPECT_GE(var, 0.0);
    EXPECT_GE(es, var);
}

TEST(MonteCarloTest, EmptySnapshotThrows) {
    MarketSnapshot snap;
    Portfolio p;
    p.instruments.push_back({InstrumentType::STOCK, "AAPL", 10});

    EXPECT_THROW({
        MonteCarloEngine mc(snap, p, 10);
    }, std::runtime_error);
}

TEST(MonteCarloTest, OptionSupport) {
    MarketSnapshot snap;
    snap.tickers = {"AAPL"};
    snap.spot["AAPL"] = 150.0;
    snap.mu = {0.0};
    snap.sigma = {0.3};
    snap.corr = {{1.0}};

    Portfolio p;
    Instrument opt;
    opt.type = InstrumentType::OPTION;
    opt.ticker = "AAPL";
    opt.quantity = 1;
    opt.strike = 100;
    opt.maturity = 1.0;
    opt.option_type = "CALL";
    p.instruments.push_back(opt);

    MonteCarloEngine mc(snap, p, 10);

    double var, es;
    mc.compute(3000, 0.95, var, es);

    EXPECT_GE(var, 0.0);
    EXPECT_GE(es, var);
}
