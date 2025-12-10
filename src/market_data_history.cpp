#include "market_data_history.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace fs = std::filesystem;

void MarketDataHistory::load_directory(const std::string& path) {
    for (const auto& entry : fs::directory_iterator(path)) {
        if (entry.path().extension() == ".csv") {
            std::string ticker = entry.path().stem().string();
            std::ifstream f(entry.path());
            if (!f.is_open()) continue;

            std::string line;
            getline(f, line); // skip header

            while (getline(f, line)) {
                std::stringstream ss(line);
                std::string date, open, high, low, close, adj, vol;
                getline(ss, date, ',');
                getline(ss, open, ',');
                getline(ss, high, ',');
                getline(ss, low, ',');
                getline(ss, close, ',');
                getline(ss, adj, ',');
                getline(ss, vol, ',');

                prices[ticker][date] = std::stod(close);
            }
        }
    }
}

double MarketDataHistory::get_price(const std::string& ticker, const std::string& date) const {
    auto it = prices.find(ticker);
    if (it == prices.end()) throw std::runtime_error("No ticker " + ticker);

    auto jt = it->second.find(date);
    if (jt == it->second.end()) throw std::runtime_error("No price for " + ticker + " on " + date);

    return jt->second;
}

std::vector<std::string> MarketDataHistory::get_future_dates(
    const std::string& snapshot_date, int horizon_days) const 
{
    std::vector<std::string> dates;

    auto any = prices.begin()->second;
    int count = 0;

    for (auto it = any.upper_bound(snapshot_date); it != any.end() && count < horizon_days; ++it) {
        dates.push_back(it->first);
        count++;
    }
    return dates;
}

std::vector<std::string> MarketDataHistory::get_past_dates(
    const std::string& snapshot_date, int lookback_days) const
{
    std::vector<std::string> dates;
    auto any = prices.begin()->second;

    auto it = any.lower_bound(snapshot_date);
    if (it == any.begin()) return dates;

    int count = 0;
    while (it != any.begin() && count < lookback_days) {
        --it;
        dates.push_back(it->first);
        count++;
    }

    return dates;
}

std::unordered_map<std::string, std::vector<std::string>> 
MarketDataHistory::get_all_dates() const {
    std::unordered_map<std::string, std::vector<std::string>> out;

    for (const auto& [ticker, mp] : prices) {
        std::vector<std::string> dates;
        dates.reserve(mp.size());
        for (const auto& [d, _] : mp) dates.push_back(d);
        out[ticker] = dates;
    }

    return out;
}
