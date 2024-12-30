#pragma once

#include <vector>
#include <string> 
#include <map>
#include <iostream>
#include "OrderBook.h"
#include "OrderBookEntry.h"

/** struct to handle candles data structure */
struct Candlestick {
    std::string timestamp;
    double open;
    double close;
    double high;
    double low;
};

class CandleStick
{
    public:
        /** Default constructor */
        CandleStick();
        /** Calculate and return candles values */
        static std::map<std::string, std::vector<Candlestick>> calculateCandlestickDetails(OrderBook& orderBook, std::string& timestamp, OrderBookType orderType, std::vector<OrderBookEntry>& selectedOrders);

        /** Filter timestamp to be displayed on x-axis*/
        static std::string filterTimestamp(std::string& timestamp);

        // ANSI color codes
        const std::string ANSI_RESET = "\033[0m";
        const std::string ANSI_GREEN = "\033[32m";
        const std::string ANSI_RED = "\033[31m";
        // Green background
        const std::string ANSI_GREEN_BG = "\033[42m";  
        // Red background
        const std::string ANSI_RED_BG = "\033[41m";  

        /** Process ANSI colours on texts*/
        void printColoredCandle(const std::string& textColor, const std::string& backgroundColor, const std::string& text);

        /** Function to print candles*/
        void printCandlestick(const Candlestick& candlestick);

    private:
        OrderBook orderBook;
        std::vector<OrderBookEntry> selectedOrders;
};