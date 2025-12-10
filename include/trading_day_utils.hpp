#pragma once
#include <string>
#include <vector>
#include <unordered_map>


/**
 * @brief Finds the nearest trading date common to all tickers
 *        that does not exceed the given target date.
 *
 * Used for:
 *   - aligning inconsistent historical datasets,
 *   - ensuring snapshot_date exists for all tickers,
 *   - DJIA portfolio generation when a day is missing.
 *
 * @param ticker_dates Map: ticker → list of sorted trading dates.
 * @param target Target date (YYYY-MM-DD).
 *
 * @return Latest common date ≤ target.
 *
 * @throws std::runtime_error if:
 *   - no tickers provided,
 *   - there is no common trading day,
 *   - target date precedes all common dates.
 */
std::string find_common_previous_date(
    const std::unordered_map<std::string, std::vector<std::string>>& ticker_dates,
    const std::string& target
);
