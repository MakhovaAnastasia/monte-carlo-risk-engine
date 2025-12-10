#include <gtest/gtest.h>
#include "realized_risk.hpp"
#include "market_data_history.hpp"
#include "portfolio.hpp"

TEST(RealizedRiskTest, BasicCase) {
    MarketDataHistory h;
    h.prices["AAPL"] = {
        {"2025-01-01", 100},
        {"2025-01-02",  90},
        {"2025-01-03", 110}
    };

    Portfolio p;
    p.instruments.push_back({InstrumentType::STOCK, "AAPL", 1});

    RealizedRisk rr = compute_realized_risk(p, h, "2025-01-01", 2);

    // Day 1: -10% loss, Day 2: +10%
    // Worst 5% → -10%  
    EXPECT_NEAR(rr.historical_var, 0.10, 1e-12);
    EXPECT_NEAR(rr.historical_es, 0.10, 1e-12);
}

TEST(RealizedRiskTest, NotEnoughDataThrows) {
    MarketDataHistory h;
    h.prices["AAPL"] = {{"2025-01-01", 100}};

    Portfolio p;
    p.instruments.push_back({InstrumentType::STOCK, "AAPL", 1});

    EXPECT_THROW(
        compute_realized_risk(p, h, "2025-01-01", 10),
        std::runtime_error
    );
}
