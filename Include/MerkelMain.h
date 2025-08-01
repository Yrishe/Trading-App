#pragma once

#include <vector>
#include "OrderBookEntry.h"
#include "OrderBook.h"
#include "Wallet.h"
#include "CandleStick.h"

class MerkelMain
{
    public:
        MerkelMain();
        /** Call this to start the sim */
        void init();
        
        /** Get current time */
        std::string getCurrentTime() const { return currentTime; }
        
        /** Get order book reference */
        OrderBook& getOrderBook() { return orderBook; }
        const OrderBook& getOrderBook() const { return orderBook; }
        
        /** Move to next timeframe */
        void gotoNextTimeframe();
        
        /** Get market statistics */
        void getMarketStats(const std::string& product, double& currentPrice, double& volume, double& change);
        
        /** Execute a trade order */
        bool executeOrder(const std::string& product, OrderBookType orderType, double price, double amount, const std::string& username);
        
        /** Get current prices for a product */
        std::vector<double> getCurrentPrices(const std::string& product, OrderBookType orderType);
        
    private: 
        void draw();
        void presentation();
        void printMenu();
        void printHelp();
        void printMarketStats();
        void enterAsk();
        void enterBid();
        void printWallet();
        void printCandleChart();
        void getPrices();
        int getUserOption();
        void processUserOption(int userOption);

        std::string currentTime;

        OrderBook orderBook{"20200601.csv"};

        Wallet wallet;
        // object of CandleStick class
        CandleStick candle;
        // object of Candlestick structure
        Candlestick candles;
};
