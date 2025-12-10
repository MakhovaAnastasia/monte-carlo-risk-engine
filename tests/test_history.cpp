#include <gtest/gtest.h>
#include "market_data_history.hpp"
#include <fstream>

static void write_history_csv(const std::string& ticker, const std::string& data) {
    std::ofstream f("data_" + ticker + ".csv");
    f << "Date,Open,High,Low,Close,Adj Close,Volume\n";
    f << data;
}

TEST(HistoryTest, LoadAndQuery) {
    write_history_csv("AAPL",
        "2025-01-01,1,1,1,100,100,1000\n"
        "2025-01-02,1,1,1,110,110,1000\n");

    MarketDataHistory h;
    h.load_directory(".");

    EXPECT_NEAR(h.get_price("AAPL", "2025-01-01"), 100, 1e-12);
    EXPECT_NEAR(h.get_price("AAPL", "2025-01-02"), 110, 1e-12);
}

TEST(HistoryTest, MissingTickerThrows) {
    MarketDataHistory h;
    EXPECT_THROW(h.get_price("XXX", "2025-01-01"), std::runtime_error);
}

TEST(HistoryTest, MissingDateThrows) {
    write_history_csv("AAPL", "2025-01-01,1,1,1,100,100,1000\n");

    MarketDataHistory h;
    h.load_directory(".");

    EXPECT_THROW(h.get_price("AAPL", "2025-01-10"), std::runtime_error);
}
