#include "djia_builder.hpp"
#include "market_data_history.hpp"

#include <fstream>
#include <iostream>
#include <unordered_map>
#include <filesystem>

namespace fs = std::filesystem;

// Dow Jones tickers
static const std::vector<std::string> DJIA_TICKERS = {
    "GS","CAT","MSFT","AXP","HD","SHW","V","UNH","AMGN","JPM",
    "MCD","IBM","TRV","AAPL","CRM","AMZN","JNJ","BA","HON","NVDA",
    "MMM","CVX","PG","WMT","DIS","MRK","CSCO","KO","NKE","VZ"
};

// ---------------------------------------------------------------------
// Helper: read Close price for given date from a Yahoo CSV file
// ---------------------------------------------------------------------
static double read_price_yahoo(const std::string& file, const std::string& target_date)
{
    std::ifstream f(file);
    if (!f.is_open())
        throw std::runtime_error("Cannot open history file: " + file);

    std::string line;
    std::getline(f, line); // skip header

    while (std::getline(f, line))
    {
        if (line.size() < 10) continue;

        // Extract date (first column)
        std::string date = line.substr(0, 10);
        if (date == target_date)
        {
            // Yahoo format: Date,Open,High,Low,Close,Adj Close,Volume
            //               0    1     2    3    4     5          6
            size_t p1 = line.find(',', 11);
            size_t p2 = line.find(',', p1 + 1);
            size_t p3 = line.find(',', p2 + 1);
            size_t p4 = line.find(',', p3 + 1);

            std::string close_str = line.substr(p3 + 1, p4 - p3 - 1);
            return std::stod(close_str);
        }
    }

    throw std::runtime_error("No price for " + file + " at date " + target_date);
}

// ---------------------------------------------------------------------
// Main function: generate the DJIA portfolio CSV
// ---------------------------------------------------------------------
bool build_djia_portfolio(const DjiaPortfolioSpec& spec)
{
    if (!fs::exists(spec.history_dir))
    {
        std::cerr << "ERROR: history directory not found: " << spec.history_dir << "\n";
        return false;
    }

    std::unordered_map<std::string,double> price;
    double total_sum = 0.0;

    // Load prices for each ticker
    for (const auto& t : DJIA_TICKERS)
    {
        std::string file = spec.history_dir + "/" + t + ".csv";

        try {
            double p = read_price_yahoo(file, spec.snapshot_date);
            price[t] = p;
            total_sum += p;
        }
        catch (const std::exception& ex)
        {
            std::cerr << "Error reading " << t << ": " << ex.what() << "\n";
            return false;
        }
    }

    // Compute quantities
    std::unordered_map<std::string,int> qty;
    for (const auto& t : DJIA_TICKERS)
    {
        double weight = price[t] / total_sum;
        double alloc  = spec.portfolio_value * weight;

        qty[t] = static_cast<int>(alloc / price[t]);
    }

    // Save portfolio CSV
    std::ofstream out(spec.output_file);
    if (!out.is_open()) {
        std::cerr << "Cannot open output: " << spec.output_file << "\n";
        return false;
    }

    out << "type,ticker,quantity,strike,maturity,option_type\n";

    for (const auto& t : DJIA_TICKERS)
    {
        out << "STOCK," << t << "," << qty[t] << ",,,\n";
    }

    return true;
}
