#include <gtest/gtest.h>
#include "portfolio.hpp"
#include <fstream>

static std::string write_temp_csv(const std::string& name, const std::string& data) {
    std::ofstream f(name);
    f << data;
    return name;
}

TEST(PortfolioTest, EmptyPortfolio) {
    Portfolio p;
    EXPECT_TRUE(p.instruments.empty());
}

TEST(PortfolioTest, LoadValidPortfolio) {
    std::string csv =
        "type,ticker,quantity,strike,maturity,option_type\n"
        "STOCK,AAPL,10,,,\n"
        "OPTION,MSFT,5,300,0.5,CALL\n";

    std::string path = write_temp_csv("p_valid.csv", csv);

    Portfolio p;
    p.load(path);

    ASSERT_EQ(p.instruments.size(), 2);

    EXPECT_EQ(p.instruments[0].type, InstrumentType::STOCK);
    EXPECT_EQ(p.instruments[0].ticker, "AAPL");
    EXPECT_EQ(p.instruments[0].quantity, 10);

    EXPECT_EQ(p.instruments[1].type, InstrumentType::OPTION);
    EXPECT_EQ(p.instruments[1].ticker, "MSFT");
    EXPECT_EQ(p.instruments[1].strike, 300);
    EXPECT_EQ(p.instruments[1].maturity, 0.5);
    EXPECT_EQ(p.instruments[1].option_type, "CALL");
}

TEST(PortfolioTest, UnknownInstrumentType) {
    std::string csv =
        "type,ticker,quantity,strike,maturity,option_type\n"
        "FUT,ES,1,,,\n";

    std::string path = write_temp_csv("p_unknown.csv", csv);

    Portfolio p;

    EXPECT_THROW(p.load(path), std::runtime_error);
}
