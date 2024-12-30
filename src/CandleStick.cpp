#include "CandleStick.h"
#include "CSVReader.h"
#include <map>
#include <iostream>
#include <fstream>
// library to manipulate strings as streams
#include <sstream>
// library to manipulate input & outputs
#include <iomanip>
#include <vector>
#include <algorithm>

// Default constructor
CandleStick::CandleStick()
{

};

std::string CandleStick::filterTimestamp(std::string& timestamp)
{
    // Parse the timestamp
    std::istringstream fTime(timestamp);
    /** variables to hold parts of the timestamp */
    int year, month, day, hour, minute, second;
    /** variables to represent the existing separators in timestamp */
    char delimiter1, delimiter2, delimiter3, dot;
    
    // Assigning values from fTime into the variables
    fTime >> year >> delimiter1 >> month >> delimiter2 >> day
        >> hour >> delimiter3 >> minute >> dot >> second;

    // Check for successful parsing
    if (fTime.fail()) {
        return "Failed to parse the timestamp."; // Return an error code
    }

    // Format of filtered timestamp
    std::ostringstream foTime;
    foTime << std::setfill('0') << std::setw(2) << hour << ":" 
        << std::setfill('0') << std::setw(2) << minute << ":" 
        << std::setfill('0') << std::setw(2) << second;

    // Retrieving formatted timestamp from foTime as string
    std::string formattedTimestamp = foTime.str();

    return foTime.str();
}

std::map<std::string, std::vector<Candlestick>> CandleStick::calculateCandlestickDetails(OrderBook& orderBook, std::string& timestamp, OrderBookType orderType, 
std::vector<OrderBookEntry>& selectedOrders) {
    /** String variable to hold timestamp of current order */
    std::string time = CandleStick::filterTimestamp(timestamp);
    /** Vector to storing key(product)>Value(time, openValue, closeValue, highestValue, lowestValue) */
    std::map<std::string, std::vector<Candlestick>> candlesticksByProduct;

    // Handling unselected products
    if (selectedOrders.empty()) {
        return candlesticksByProduct; // Return an empty map
    }

    // Iterate through selected order and perform calculations
    for (const auto& order : selectedOrders) {
        // Use the product from the order
        std::string product = order.product;

        // Check if data for the product has already been stored
        if (candlesticksByProduct.find(product) == candlesticksByProduct.end()) {
                // Create a new vector to selected product to store found data
                candlesticksByProduct[product] = std::vector<Candlestick>();
                // Calculate highest and lowest values for the product
                double highestValue = orderBook.getHighPrice(selectedOrders, product);
                double lowestValue = orderBook.getLowPrice(selectedOrders, product);

                // Check if there is a previous close value for the product
                double openValue = 0.0;
                if (!candlesticksByProduct[product].empty()) {
                    // Use the close value of the last candlestick as the open value for the current candlestick
                    openValue = candlesticksByProduct[product].back().close;
                }
                // Use the current timestamp's average price as the close value
                double closeValue = orderBook.getAveragePrice(selectedOrders, product); 

                // Create a Candlestick instance and add it to the vector for the specific product
                Candlestick candlestick = {time, openValue, closeValue, highestValue, lowestValue};
                // push the object associated with product into the vector
                candlesticksByProduct[product].push_back(candlestick);
            }
        }
    return candlesticksByProduct;
}

void CandleStick::printColoredCandle(const std::string& textColor, const std::string& backgroundColor, const std::string& text) {
    // Print the colored block
    std::cout << textColor << backgroundColor << text<< ANSI_RESET << "|" << textColor << backgroundColor << text << ANSI_RESET << "         ";
}

void CandleStick::printCandlestick(const Candlestick& candlestick) {
    // Use the ANSI escape codes directly in the printColoredBlock calls
    std::string fillChar = candlestick.close >= candlestick.open ? "-" : "-";

    // Print the candlestick
    for (int i = int(candlestick.low); i <= int(candlestick.high); ++i) {

        // Check if the current iteration value corresponds to the open or close price
        if (i == int(candlestick.open) || i == int(candlestick.close)) {
            for (char c : fillChar) {
                // Use the adjusted x-axis position
                if (candlestick.close >= candlestick.open) {
                    // positive = green
                    printColoredCandle(ANSI_GREEN, ANSI_GREEN_BG, std::string(2, c));
                } else {
                    // negative = red
                    printColoredCandle(ANSI_RED, ANSI_RED_BG, std::string(2, c));
                }
            }
        } else {
            // Print spaces before printing the character
            std::cout << "  ";
        }
    }
}
