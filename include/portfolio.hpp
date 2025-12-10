#pragma once
#include <vector>
#include <string>

/**
 * @brief Type of financial instrument supported by the system.
 */
enum class InstrumentType { STOCK, OPTION };

/**
 * @brief Represents a single portfolio position.
 *
 * STOCK fields:
 *   - ticker
 *   - quantity
 *
 * OPTION fields:
 *   - strike
 *   - maturity (not used yet in pricing)
 *   - option_type: "CALL" or "PUT"
 */
struct Instrument {
    InstrumentType type;
    std::string ticker;
    int quantity;

    // option-specific
    double strike = 0.0; ///< option strike price
    double maturity = 0.0;  ///< option maturity in years
    std::string option_type; // CALL or PUT
};

/**
 * @brief Portfolio consisting of stocks and European-style options.
 *
 * Provides loading from CSV format:
 *   type,ticker,quantity,strike,maturity,option_type
 *
 * Unknown instrument types trigger exceptions.
 */
class Portfolio {
public:
    std::vector<Instrument> instruments;

    /**
     * @brief Loads portfolio positions from a CSV file.
     *
     * @param file Path to portfolio CSV.
     *
     * @throws std::runtime_error if:
     *   - file cannot be opened,
     *   - row format is invalid,
     *   - instrument type is unknown.
     */
    void load(const std::string& file);
};
