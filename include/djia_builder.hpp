#pragma once
#include <string>
/**
 * @brief Configuration parameters for automatic DJIA portfolio construction.
 *
 * This struct specifies:
 *  - snapshot_date: date on which the portfolio should be generated
 *  - portfolio_value: total notional value to allocate across Dow Jones stocks
 *  - history_dir: directory containing Yahoo-format historical CSV files
 *  - output_file: resulting CSV file where generated portfolio will be stored
 */
struct DjiaPortfolioSpec {
    std::string snapshot_date;
    double portfolio_value = 10'000'000;
    std::string history_dir;
    std::string output_file;
};


/**
 * @brief Builds a synthetic Dow Jones (DJIA) stock-only portfolio.
 *
 * The portfolio is constructed using price-weighted DJIA methodology:
 *  - loads historical data for all 30 DJIA tickers,
 *  - extracts closing prices on the given snapshot date,
 *  - assigns weights proportional to stock prices,
 *  - allocates total portfolio_value according to weights,
 *  - writes result into a CSV file.
 *
 * @param spec Structure describing portfolio generation parameters.
 *
 * @return true on success, false if any ticker is missing
 *         or historical data is unavailable.
 *
 * @throws No exceptions are thrown directly; errors are printed to stderr.
 */
bool build_djia_portfolio(const DjiaPortfolioSpec& spec);
