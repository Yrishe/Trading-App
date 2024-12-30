#include "OrderBook.h"
#include "CSVReader.h"
#include <map>
#include <algorithm>
#include <iostream>

#include <unordered_set>

OrderBook::OrderBook()
{

};

/** construct, reading a csv data file */
OrderBook::OrderBook(std::string filename)
{
    orders = CSVReader::readCSV(filename);
}

/** return vector of all know products in the dataset*/
std::vector<std::string> OrderBook::getKnownProducts()
{
    std::vector<std::string> products;

    std::map<std::string,bool> prodMap;

    for (OrderBookEntry& e : orders)
    {
        prodMap[e.product] = true;
    }
    
    // now flatten the map to a vector of strings
    for (auto const& e : prodMap)
    {
        products.push_back(e.first);
    }

    return products;
}

/** Return known timestamps from an x product */
std::vector<std::string> OrderBook::getKnownTimestamps(std::string product)
{
    std::vector<std::string> timestamps;

    std::map<std::string, bool> timeMap;

    for (const OrderBookEntry& e : orders)
    {
        if (e.product == product)
        {
            timeMap[e.timestamp] = true;
        }
    }

    // now flatten the map to a vector of strings
    for (const auto& e : timeMap)
    {
        timestamps.push_back(e.first);
    }

    return timestamps;
}

/** return vector of Orders according to the sent filters*/
std::vector<OrderBookEntry> OrderBook::getOrders(OrderBookType type, 
                                        std::string product, 
                                        std::string timestamp)
{
    std::vector<OrderBookEntry> orders_sub;
    for (OrderBookEntry& e : orders)
    {
        if (e.orderType == type && 
            e.product == product && 
            e.timestamp == timestamp )
            {
                orders_sub.push_back(e);
            }
    }
    return orders_sub;
}

double OrderBook::getHighPrice(std::vector<OrderBookEntry>& orders, std::string product)
{
    double max = orders[0].price;
    std::string identity = orders[0].product;
    for (OrderBookEntry& e : orders)
    {
        if (e.price > max)
        {
            max = e.price;
            identity = e.product;
        }
    }
    return max;
}


double OrderBook::getLowPrice(std::vector<OrderBookEntry>& orders, std::string product)
{
    double min = orders[0].price;
    std::string identity = orders[0].product;
    for (OrderBookEntry& e : orders)
    {
        if (e.price < min) {
            min = e.price;
            identity = e.product;
        }
    }
    return min;
}

double OrderBook::getAveragePrice(std::vector<OrderBookEntry>& orders, std::string product)
{
    float sum = 0.0;
    int count = 0;
    std::string identity;
    for (const OrderBookEntry& e : orders)
    {
        if (e.product == product)
        {
            sum += e.price;
            count++;
            identity = e.product; // Assuming you want the product of the last matching entry
        }
    }
    // Avoid division by zero
    if (count == 0) std::cout<<"OrderBook::getAveragePrice - There is no match"<<std::endl;

    float average = sum / count;

    return average;
}

std::string OrderBook::getEarliestTime()
{
    return orders[0].timestamp;
}

std::string OrderBook::getNextTime(std::string timestamp)
{
    std::string next_timestamp = "";
    for (OrderBookEntry& e : orders)
    {
        if (e.timestamp > timestamp) 
        {
            next_timestamp = e.timestamp;
            break;
        }
    }
    if (next_timestamp == "")
    {
        next_timestamp = orders[0].timestamp;
    }
    return next_timestamp;
}

// Return ROI(Return on investment)
double OrderBook::getROI(std::vector<OrderBookEntry>& orders)
{
    double in = OrderBook::getLowPrice(orders, orders[0].product);
    double out = OrderBook::getHighPrice(orders, orders[0].product);
    signed int sell = out-in;
    double ROI = ((sell-in)/in)*100;
    return ROI;
};


void OrderBook::insertOrder(OrderBookEntry& order)
{
    orders.push_back(order);
    std::sort(orders.begin(), orders.end(), OrderBookEntry::compareByTimestamp);
}

std::vector<OrderBookEntry> OrderBook::matchAsksToBids(std::string product, std::string timestamp)
{
// asks = orderbook.asks
    std::vector<OrderBookEntry> asks = getOrders(OrderBookType::ask, 
                                                 product, 
                                                 timestamp);
// bids = orderbook.bids
    std::vector<OrderBookEntry> bids = getOrders(OrderBookType::bid, 
                                                 product, 
                                                    timestamp);

    // sales = []
    std::vector<OrderBookEntry> sales; 

    // sort asks lowest first
    std::sort(asks.begin(), asks.end(), OrderBookEntry::compareByPriceAsc);
    // sort bids highest first
    std::sort(bids.begin(), bids.end(), OrderBookEntry::compareByPriceDesc);
    // for ask in asks:
    std::cout << "max ask " << asks[asks.size()-1].price << std::endl;
    std::cout << "min ask " << asks[0].price << std::endl;
    std::cout << "max bid " << bids[0].price << std::endl;
    std::cout << "min bid " << bids[bids.size()-1].price << std::endl;
    
    for (OrderBookEntry& ask : asks)
    {
        //std::cout << "Ask p: " << ask.price << " a: " << ask.amount << std::endl;
    //     for bid in bids:
        for (OrderBookEntry& bid : bids)
        {
            //std::cout << "bid p: " << bid.price << " a: " << bid.amount << std::endl;

    //         if bid.price >= ask.price # we have a match
            if (bid.price >= ask.price)
            {
    //             sale = new order()
    //             sale.price = ask.price
                OrderBookEntry sale{ask.price, 0, timestamp, product, OrderBookType::asksale};
                if(bid.username == "simuser")
                {
                    sale.username = "simuser";
                    sale.orderType = OrderBookType::bidsale;
                }
                if(ask.username == "simuser")
                {
                    sale.username = "simuser";
                    sale.orderType = OrderBookType::asksale;
                }
    //             # now work out how much was sold and 
    //             # create new bids and asks covering 
    //             # anything that was not sold
    //             if bid.amount == ask.amount: # bid completely clears ask
                if (bid.amount == ask.amount)
                {
    //                 sale.amount = ask.amount
                    sale.amount = ask.amount;
    //                 sales.append(sale)
                    sales.push_back(sale);
    //                 bid.amount = 0 # make sure the bid is not processed again
                    bid.amount = 0;
    //                 # can do no more with this ask
    //                 # go onto the next ask
    //                 break
                    break;
                }
    //           if bid.amount > ask.amount:  # ask is completely gone slice the bid
                if (bid.amount > ask.amount)
                {
    //                 sale.amount = ask.amount
                    sale.amount = ask.amount;
    //                 sales.append(sale)
                    sales.push_back(sale);
    //                 # we adjust the bid in place
    //                 # so it can be used to process the next ask
    //                 bid.amount = bid.amount - ask.amount
                    bid.amount =  bid.amount - ask.amount;
    //                 # ask is completely gone, so go to next ask                
    //                 break
                    break;
                }


    //             if bid.amount < ask.amount # bid is completely gone, slice the ask
                if (bid.amount < ask.amount && bid.amount > 0)
                {
    //                 sale.amount = bid.amount
                    sale.amount = bid.amount;
    //                 sales.append(sale)
                    sales.push_back(sale);
    //                 # update the ask
    //                 # and allow further bids to process the remaining amount
    //                 ask.amount = ask.amount - bid.amount
                    ask.amount = ask.amount - bid.amount;
    //                 bid.amount = 0 # make sure the bid is not processed again
                    bid.amount = 0;
    //                 # some ask remains so go to the next bid
    //                 continue
                    continue;
                }
            }
        }
    }
    return sales;             
}

