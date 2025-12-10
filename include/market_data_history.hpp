#pragma once
#include <string>
#include <unordered_map>
#include <map>
#include <vector>

/**
 * @brief Container for historical daily price data for multiple tickers.
 *
 * Stores:
 *   prices[ticker][date] = close price
 *
 * Provides helper functions for:
 *   - loading CSV files from a directory,
 *   - accessing individual prices,
 *   - retrieving past/future date ranges,
 *   - extracting available dates for each ticker.
 */

class MarketDataHistory {
public:
    // map: ticker -> (date -> close price)
    std::unordered_map<std::string, std::map<std::string, double>> prices;

    /**
     * @brief Loads all *.csv files from a directory.
     *
     * CSV format (Yahoo-style):
     *   Date,Open,High,Low,Close,Adj Close,Volume
     *
     * Ticker name is taken from filename (e.g., AAPL.csv → AAPL).
     *
     * @param path Directory containing historical CSV files.
     */
    void load_directory(const std::string& path);


    /**
     * @brief Returns the close price for a given ticker and date.
     *
     * @param ticker Ticker symbol.
     * @param date Date in YYYY-MM-DD format.
     *
     * @throws std::runtime_error if ticker or date is missing.
     */
    double get_price(const std::string& ticker, const std::string& date) const;

    /**
     * @brief Returns a list of the next horizon_days trading dates
     *        after snapshot_date.
     *
     * Used to compute realized (historical) VaR/ES.
     */

    std::vector<std::string> get_future_dates(
        const std::string& snapshot_date,
        int horizon_days
    ) const;

    /**
     * @brief Returns a list of lookback_days dates preceding snapshot_date.
     *
     * Used to estimate drift, volatility, and correlations.
     */

    std::vector<std::string> get_past_dates(
        const std::string& snapshot_date,
        int lookback_days
    ) const;

    /**
     * @brief Returns all available trading dates for every ticker.
     *
     * @return Map ticker → array of available dates.
     */
    std::unordered_map<std::string, std::vector<std::string>> get_all_dates() const;

};
