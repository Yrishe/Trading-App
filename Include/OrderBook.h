#pragma once
#include "OrderBookEntry.h"
#include "CSVReader.h"
#include <string>
#include <vector>
#include <utility>

class OrderBook
{
    public:
        OrderBook();
        /** construct, reading a csv data file */
        OrderBook(std::string filename);
        /** return vector of all know products in the dataset*/
        std::vector<std::string> getKnownProducts();
        /** return vector of Orders according to the sent filters*/
        std::vector<OrderBookEntry> getOrders(OrderBookType type, 
                                              std::string product, 
                                              std::string timestamp);

        /** returns the earliest time in the orderbook*/
        std::string getEarliestTime();

        // double getOpeningPrice();
        std::vector<std::string> getKnownTimestamps(std::string product);

        /** returns the next time after the 
         * sent time in the orderbook  
         * If there is no next timestamp, wraps around to the start
         * */
        std::string getNextTime(std::string timestamp);
        void insertOrder(OrderBookEntry& order);

        std::vector<OrderBookEntry> matchAsksToBids(std::string product, std::string timestamp);

        static double getHighPrice(std::vector<OrderBookEntry>& orders, std::string product);
        static double getLowPrice(std::vector<OrderBookEntry>& orders, std::string product);

        /** Calculate close values*/
        static double getAveragePrice(std::vector<OrderBookEntry>& orders, std::string product);

        // get ROI
        static double getROI(std::vector<OrderBookEntry>& orders);
   
    private:
        std::vector<OrderBookEntry> orders;
};
