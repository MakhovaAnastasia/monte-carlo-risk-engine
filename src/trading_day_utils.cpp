#include "trading_day_utils.hpp"
#include <algorithm>
#include <stdexcept>

std::string find_common_previous_date(
    const std::unordered_map<std::string, std::vector<std::string>>& ticker_dates,
    const std::string& target)
{
    if (ticker_dates.empty())
        throw std::runtime_error("No tickers provided to date alignment");

    bool first = true;
    std::vector<std::string> intersection;

    // === 1. Compute intersection of all dates ===
    for (const auto& [ticker, dates] : ticker_dates) {
        if (first) {
            intersection = dates;
            first = false;
        } else {
            std::vector<std::string> temp;
            std::set_intersection(
                intersection.begin(), intersection.end(),
                dates.begin(), dates.end(),
                std::back_inserter(temp)
            );
            intersection = std::move(temp);
        }
    }

    if (intersection.empty())
        throw std::runtime_error("No common dates across tickers!");

    // === 2. Find nearest date <= target ===
    std::string best = "";
    for (const auto& d : intersection) {
        if (d <= target)
            best = d;
        else
            break;
    }

    if (best.empty())
        throw std::runtime_error("No common date <= target (" + target + ")");

    return best;
}
