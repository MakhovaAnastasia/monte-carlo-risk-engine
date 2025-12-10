#include "portfolio.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>

static std::vector<std::string> split(const std::string& line) {
    std::vector<std::string> out;
    std::stringstream ss(line);
    std::string cell;

    while (std::getline(ss, cell, ',')) {
        out.push_back(cell);
    }
    return out;
}

void Portfolio::load(const std::string& file) {
    std::ifstream f(file);
    if (!f.is_open()) {
        throw std::runtime_error("Cannot open portfolio file: " + file);
    }

    instruments.clear();
    std::string line;

    // skip header
    std::getline(f, line);

    while (std::getline(f, line)) {
        if (line.empty())
            continue;

        auto row = split(line);
        if (row.size() < 3)
            throw std::runtime_error("Invalid portfolio row: " + line);

        Instrument inst;

        // ----- TYPE -----
        if (row[0] == "STOCK") {
            inst.type = InstrumentType::STOCK;
            inst.ticker = row[1];
            inst.quantity = std::stoi(row[2]);
        }
        else if (row[0] == "OPTION") {
            if (row.size() < 6)
                throw std::runtime_error("Invalid OPTION row: " + line);

            inst.type = InstrumentType::OPTION;
            inst.ticker = row[1];
            inst.quantity = std::stoi(row[2]);
            inst.strike = std::stod(row[3]);
            inst.maturity = std::stod(row[4]);
            inst.option_type = row[5]; // CALL / PUT
        }
        else {
            throw std::runtime_error("Unknown instrument type: " + row[0]);
        }

        instruments.push_back(inst);
    }
}
