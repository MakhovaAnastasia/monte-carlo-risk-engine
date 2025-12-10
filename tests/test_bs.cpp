#include <gtest/gtest.h>
#include "bs_model.hpp"
#include "instrument.hpp"

TEST(BlackScholesTest, ATMCall) {
    BlackScholesModel model;

    Instrument inst;
    inst.type = InstrumentType::OPTION;
    inst.option_type = "CALL";
    inst.strike = 100;

    PricingContext ctx{100, 0.2, 0.0, 1.0};

    double price = model.price(inst, ctx);

    EXPECT_NEAR(price, 7.9656, 0.01);
}
