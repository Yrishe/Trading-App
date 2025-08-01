#include "MerkelMain.h"
#include <iostream>
#include <vector>
#include <iomanip>
#include "OrderBookEntry.h"
#include "CSVReader.h"

MerkelMain::MerkelMain()
{

};

void MerkelMain::init()
{
    currentTime = orderBook.getEarliestTime();

    wallet.insertCurrency("BTC", 10);
    wallet.insertCurrency("ETH", 10);
    wallet.insertCurrency("USDT", 10);
    wallet.insertCurrency("DOGE", 10);

    // For desktop app, we don't run the console loop
    // The GUI will handle user interactions
}
// Draw welcome 
void MerkelMain::draw()
{
    std::cout << "  -----------------------------------------------------------" << std::endl;
    std::cout << "|                                                             |" << std::endl;
    std::cout << "| " << "ETH" << "        ETH" << " DOGEDO" << " BT" << "       USDT" << "  ETHT" << "  DOGE" << "  DOGE" << " USDTU" << " |" << std::endl;
    std::cout << "| " << "ETH" << "        ETH"<< " DO"<< "     BT" << "     US" << "     E" << "    E" << " DO DOGE DO" << " US" << "    |" << std::endl;
    std::cout << "| " << "ETH" << "  ETH" << "   ETH" << " DOGED" << "  BT" << "     US"<< "     E" << "    E"<< " DO  DO  DO" << " USDT" << "  |" << std::endl;
    std::cout << "| " << "ETH" << " ETH" << "ETH ETH" << " DO" << "     BT" << "     US" << "     E" << "    E" << " DO" << "      DO" << " US" << "    |" << std::endl;
    std::cout << "| " << "ETH" << "ETH" << "  ETHETH" << " DOGEDO"<< " BTCBTC" << "   USDT"<< "  ETHT" << "  DO" << "      DO"<< " USDTU" << " |" << std::endl;
    std::cout << "|                                                             |" << std::endl;
    std::cout << "  -----------------------------------------------------------" << std::endl;
}
// Call initial menu in order right after "Welcome"
void MerkelMain::presentation()
{
    int input;
    printMenu();
    input = getUserOption();
    processUserOption(input);
}

void MerkelMain::printMenu()
{
    std::cout << "\n" << "========================= " << std::endl;
    std::cout << "          MENU" << std::endl;
    // 1 print help
    std::cout << "\n" << "1: Print help " << std::endl;
    // 2 print exchange stats
    std::cout << "2: Print exchange stats" << std::endl;
    // 3 make an offer
    std::cout << "3: Make an ask " << std::endl;
    // 4 make a bid 
    std::cout << "4: Make a bid " << std::endl;
    // 5 print wallet
    std::cout << "5: Print wallet " << std::endl;
    // 6 continue   
    std::cout << "6: Print Candle Chart " << std::endl;
    // 7 Candles
    std::cout << "7: Get Prices" << std::endl;
    // 8 Candles
    std::cout << "8: Continue" << std::endl;
    std::cout << "\n" << "(Current time is: " << currentTime << ")" << "\n" << std::endl;
    std::cout << "=========================" << std::endl;
}

void MerkelMain::printHelp()
{
    std::cout << "Help - your aim is to make money. Analysis the market and make bids and offers. " << std::endl;
}

void MerkelMain::printMarketStats()
{
    for (std::string const& p : orderBook.getKnownProducts())
    {
        std::cout << "\n" << "Product: " << p << std::endl;
        std::vector<OrderBookEntry> entriesAsk = orderBook.getOrders(OrderBookType::ask, 
                                                                p, currentTime);
        std::vector<OrderBookEntry> entriesBid = orderBook.getOrders(OrderBookType::bid, 
        p, currentTime);
        std::cout << "Bid seen: " << entriesAsk.size() << std::endl;
        std::cout << "Max bid: " << OrderBook::getHighPrice(entriesAsk, p) << std::endl;
        std::cout << "Min bid: " << OrderBook::getLowPrice(entriesAsk, p) << std::endl;
        std::cout << "ROI: " << OrderBook::getROI(entriesAsk) << "\n" << std::endl;

        std::cout << "Bid seen: " << entriesBid.size() << std::endl;
        std::cout << "Max bid: " << OrderBook::getHighPrice(entriesBid, p) << std::endl;
        std::cout << "Min bid: " << OrderBook::getLowPrice(entriesBid, p) << std::endl;
        std::cout << "ROI: " << OrderBook::getROI(entriesBid) << "\n" << "------------------" << std::endl;
    }
}

void MerkelMain::enterAsk()
{
    std::cout << "Make an ask - enter the amount: product,price, amount, eg  ETH/BTC,200,0.5" << std::endl;
    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> tokens = CSVReader::tokenise(input, ',');
    if (tokens.size() != 3)
    {
        std::cout << "MerkelMain::enterAsk Bad input! " << input << std::endl;
    }
    else {
        try {
            OrderBookEntry obe = CSVReader::stringsToOBE(
                tokens[1],
                tokens[2], 
                currentTime, 
                tokens[0], 
                OrderBookType::ask 
            );
            // setting user name
            obe.username = "Simuser";

            if(wallet.canFulfillOrder(obe))
            {
                std::cout << "Wallet looks good." << std::endl;
                orderBook.insertOrder(obe);
            }
            else
            {
                std::cout << "Wallet has insufficient funds." << std::endl;
            }
        }catch (const std::exception& e)
        {
            std::cout << " MerkelMain::enterAsk Bad input " << std::endl;
        }   
    }
}

void MerkelMain::enterBid()
{
    std::cout << "Make an bid - enter the amount: product,price, amount, eg  ETH/BTC,200,0.5" << std::endl;
    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> tokens = CSVReader::tokenise(input, ',');
    if (tokens.size() != 3)
    {
        std::cout << "MerkelMain::enterBid Bad input! " << input << std::endl;
    }
    else {
        try {
            OrderBookEntry obe = CSVReader::stringsToOBE(
                tokens[1],
                tokens[2], 
                currentTime, 
                tokens[0], 
                OrderBookType::bid 
            );
            // setting user name
            obe.username = "Simuser";

            if(wallet.canFulfillOrder(obe))
            {
                std::cout << "Wallet looks good." << std::endl;
                orderBook.insertOrder(obe);
            }
            else
            {
                std::cout << "Wallet has insufficient funds." << std::endl;
            }
        }catch (const std::exception& e)
        {
            std::cout << " MerkelMain::enterBid Bad input " << std::endl;
        }   
    }
    std::cout << "Make a bid - enter the amount" << std::endl;
}

void MerkelMain::printWallet()
{
    std::cout << wallet.toString() << std::endl;
}
        
void MerkelMain::gotoNextTimeframe()
{
    std::cout << "Going to next time frame. " << std::endl;
    for (std::string p : orderBook.getKnownProducts())
    {
        std::cout << "matching " << p << std::endl;
        std::vector<OrderBookEntry> sales =  orderBook.matchAsksToBids(p, currentTime);
        std::cout << "Sales: " << sales.size() << std::endl;
        for (OrderBookEntry& sale : sales)
        {
            std::cout << "Sale price: " << sale.price << " amount " << sale.amount << std::endl; 
            if (sale.username == "simuser")
            {
                // update the wallet
                wallet.processSale(sale);
            }
        }
        
    }

    currentTime = orderBook.getNextTime(currentTime);
}
 
int MerkelMain::getUserOption()
{
    int userOption = 0;
    std::string line;
    std::cout << "Type in 1-7" << "\n" << std::endl;
    std::getline(std::cin, line);
    try{
        userOption = std::stoi(line);
    }catch(const std::exception& e)
    {
        std::cout << "Invalid input, try again" << std::endl;
    }
    std::cout << "\n"<< "You chose: " << userOption << "\n" << std::endl;
    return userOption;
}

void MerkelMain::printCandleChart()
{
    // array to store existing coins to validate the user's input
    std::string validProducts[] = {"ETH/BTC", "DOGE/BTC", "BTC/USDT", "ETH/USDT", "DOGE/USDT"};
    // Get user input for product and order type
    std::string userProductInput;
    std::string orderTypeStr;

    // Error handler
    try
    {
        // Ask the user to pick a product in the following format
        std::cout << "Enter the product (e.g., ETH/BTC): ";
        // get and read user's input
        std::getline(std::cin, userProductInput);
        if (std::find(std::begin(validProducts), std::end(validProducts), userProductInput) == std::end(validProducts)) {
            throw std::runtime_error("Invalid product");
        }
        else 
        {
            std::cout << "Enter the order type (bid or ask): ";
            std::cin >> orderTypeStr;

            std::cout << "\n" << std::endl;

            /** orderType check the user's request with the available options*/
            OrderBookType orderType = (orderTypeStr == "bid") ? OrderBookType::bid : OrderBookType::ask;

            /** Variables to assist operations*/
            int pricesCounter = 0;
            int timestampsCounter = 0;
            const std::string dashes(100, '-');
            std::string space(50, ' ');

            /** Print  the chart's top label */
            std::cout << space << "(" << userProductInput << ")" << " - " << orderTypeStr << "\n" << std::endl;

            /** Vector to store the matched timestamps requested from user*/
            std::vector<std::string> timestamps = orderBook.getKnownTimestamps(userProductInput);
            std::vector<double> yAxisValUp{0.02190250, 0.02190200, 0.02190150, 0.02190100, 0.02190050, 0.02190000};
            std::vector<double> yAxisValLow{0.02170250, 0.02170200, 0.02170150, 0.02170100, 0.02170050, 0.02170000};

            for(double i=0; i<yAxisValUp.size(); i++)
            {
                std::cout << std::setw(13) << std::fixed << std::setprecision(8) << yAxisValUp[i] << " | " << std::endl;
            }

            // Loop through known products
            for (const std::string& product : orderBook.getKnownProducts()) {
                /* mapped vector to store calculated candles*/
                std::map<std::string, std::vector<Candlestick>> candlesticksByProduct;

                // Filter entries based on user input
                std::vector<OrderBookEntry> selectedOrders;

                // Reset counters
                pricesCounter = 0;
                timestampsCounter = 0;

                // Loop through timestamps
                for (size_t i = 0; i < timestamps.size(); ++i) {

                    // Get orders for the specified product, order type, and timestamp
                    std::vector<OrderBookEntry> orders = orderBook.getOrders(orderType, userProductInput, timestamps[i]);

                    // Filter selected orders based on user input
                    for (const auto& entry : orders) {
                        if (entry.product == product && entry.orderType == orderType) {
                            selectedOrders.push_back(entry);
                        }
                    }

                    // Calculate candlestick details
                    candlesticksByProduct = candle.calculateCandlestickDetails(orderBook, timestamps[i], orderType, selectedOrders);

                    for (const auto& entry : candlesticksByProduct) {

                        // Iterate over the vector associated with each key
                        for (const auto& candlestick : entry.second) {
                            // pricesCounter determine how many candles on chart
                            if(pricesCounter < 6) 
                            {
                                if(pricesCounter<1)std::cout << std::setw(13) << std::fixed << std::setprecision(8) << candlestick.close << " |    ";
                                candle.printCandlestick(candlestick);
                            }
                            pricesCounter++;
                        }
                    }
                }
            }
            std::cout << std::endl;
            // displays the values after candle's line
            for(double i=0; i<yAxisValLow.size(); i++)
            {
                std::cout << std::setw(13) << std::fixed << std::setprecision(8) << yAxisValLow[i] << " | " << std::endl;
            }

            // print dashes on top of timestamps
            std::cout << dashes << std::endl;
            // Print timestamps at the bottom
            std::cout << "  Timestamps: |";
            // for (const auto& timestamp : timestamps) {
            for(int i=0; i<timestamps.size(); i++){
                // Check if the limit (6) is reached
                if (timestampsCounter >= 6) {
                    break;
                }

                // Assuming Timestamp is a string type, filter and print it
                std::cout << "   " << candle.filterTimestamp(timestamps[i]) << "   ";

                // Increment the timestamps counter
                timestampsCounter++;
            }
            std::cout << std::endl;
        }
    }
    catch(const std::exception& e)
    {
        std::cout << "\n" << "MerkelMain::printCandleChart - No such product, heading back to Menu " << '\n';
    }
}

void MerkelMain::getPrices()
{
    // array to store existing coins to validate the user's input
    std::string validProducts[] = {"ETH/BTC", "DOGE/BTC", "BTC/USDT", "ETH/USDT", "DOGE/USDT"};

    // Get user input for product and order type
    std::string userProductInput;
    std::string orderTypeStr;

        // Error handler
    try
    {
        // Ask the user to pick a product in the following format
        std::cout << "Enter the product (e.g., ETH/BTC): ";
        // get and read user's input
        std::getline(std::cin, userProductInput);
        if (std::find(std::begin(validProducts), std::end(validProducts), userProductInput) == std::end(validProducts)) {
            throw std::runtime_error("Invalid product");
        }
        else
        {
            std::cout << "Enter the order type (bid or ask): ";
            std::cin >> orderTypeStr;
            std::cout << "\n" << std::endl;

            /** orderType check the user's request with the available options*/
            OrderBookType orderType = (orderTypeStr == "bid") ? OrderBookType::bid : OrderBookType::ask;

            /** Variables to assist operations*/
            std::string space(50, ' ');

            /** Print  the chart's top label */
            std::cout << space << "(" << userProductInput << ")" << " - " << orderTypeStr << "\n" << std::endl;

            /** Vector to store the matched timestamps requested from user*/
            std::vector<std::string> times = orderBook.getKnownTimestamps(userProductInput);


            // Loop through known products
            for (const std::string& product : orderBook.getKnownProducts()) {
                /* mapped vector to store calculated candles*/
                std::map<std::string, std::vector<Candlestick>> candlesticksByProduct;

                // Filter entries based on user input
                std::vector<OrderBookEntry> selectedOrders;

                // Loop through timestamps
                for (size_t i = 0; i < times.size(); ++i) {

                    // Get orders for the specified product, order type, and timestamp
                    std::vector<OrderBookEntry> orders = orderBook.getOrders(orderType, userProductInput, times[i]);

                    // Filter selected orders based on user input
                    for (const auto& entry : orders) {
                        if (entry.product == product && entry.orderType == orderType) {
                            selectedOrders.push_back(entry);
                        }
                    }

                    // Calculate candlestick details
                    candlesticksByProduct = candle.calculateCandlestickDetails(orderBook, times[i], orderType, selectedOrders);

                    // iterate through selected product and print calculated parameters
                    for (int j = 0; j < candlesticksByProduct[product].size(); ++j) {
                        std::cout << " ([Open: " << std::setw(9) << std::fixed << std::setprecision(8) << candlesticksByProduct[product][j].open << "]";
                        std::cout << " [high: " << std::setw(9) << std::fixed << std::setprecision(8) << candlesticksByProduct[product][j].high << "]";
                        std::cout << " [low: " << std::setw(9) << std::fixed << std::setprecision(8) << candlesticksByProduct[product][j].low << "]";
                        std::cout << " [close: " << std::setw(9) << std::fixed << std::setprecision(8) << candlesticksByProduct[product][j].close << "]";
                        std::cout << " [Timestamps: " << candle.filterTimestamp(times[i]) << "])" << "\n";
                        std::cout << std::endl;
                    }
                }
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cout << "\n" << "MerkelMain::getPrices - No such product found, Heading back to Menu " << '\n';
    } 
}

void MerkelMain::processUserOption(int userOption)
{
    if (userOption == 0) // bad input
    {
        std::cout << "Invalid choice. Choose 1-6" << std::endl;
    }
    if (userOption == 1) 
    {
        printHelp();
    }
    if (userOption == 2) 
    {
        printMarketStats();
    }
    if (userOption == 3) 
    {
        enterAsk();
    }
    if (userOption == 4) 
    {
        enterBid();
    }
    if (userOption == 5) 
    {
        printWallet();
    }
    if (userOption == 6) 
    {
        printCandleChart();
    }       
    if (userOption == 7) 
    {
        getPrices();
    }
    if (userOption == 8) 
    {
        gotoNextTimeframe();
    }  
}

void MerkelMain::getMarketStats(const std::string& product, double& currentPrice, double& volume, double& change)
{
    std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::ask, product, currentTime);
    
    if (!entries.empty()) {
        currentPrice = entries[0].price;
        
        // Calculate volume (sum of all amounts)
        volume = 0.0;
        for (const auto& entry : entries) {
            volume += entry.amount;
        }
        
        // Calculate price change (simplified - compare with previous timeframe)
        std::vector<std::string> times = orderBook.getKnownProducts();
        if (times.size() > 1) {
            // For simplicity, we'll use a mock previous price calculation
            // In a real implementation, you'd need to track historical prices
            std::vector<OrderBookEntry> prevEntries = orderBook.getOrders(OrderBookType::ask, product, currentTime);
            if (!prevEntries.empty()) {
                double prevPrice = prevEntries[0].price;
                change = ((currentPrice - prevPrice) / prevPrice) * 100.0;
            } else {
                change = 0.0;
            }
        } else {
            change = 0.0;
        }
    } else {
        currentPrice = 0.0;
        volume = 0.0;
        change = 0.0;
    }
}

bool MerkelMain::executeOrder(const std::string& product, OrderBookType orderType, double price, double amount, const std::string& username)
{
    // Create order entry
    OrderBookEntry order(price, amount, currentTime, product, orderType, username);
    
    // For simulation purposes, we'll assume the order is executed immediately
    // In a real trading system, this would involve matching with existing orders
    
    // Check if user has sufficient funds (simplified)
    if (orderType == OrderBookType::bid) {
        // Buying - need sufficient quote currency (e.g., USDT)
        double totalCost = price * amount;
        // This would normally check the user's wallet
        return true; // Simplified for demo
    } else if (orderType == OrderBookType::ask) {
        // Selling - need sufficient base currency
        // This would normally check the user's wallet
        return true; // Simplified for demo
    }
    
    return false;
}

std::vector<double> MerkelMain::getCurrentPrices(const std::string& product, OrderBookType orderType)
{
    std::vector<OrderBookEntry> entries = orderBook.getOrders(orderType, product, currentTime);
    std::vector<double> prices;
    
    for (const auto& entry : entries) {
        prices.push_back(entry.price);
    }
    
    // Sort prices
    if (orderType == OrderBookType::ask) {
        std::sort(prices.begin(), prices.end()); // Ascending for asks
    } else {
        std::sort(prices.rbegin(), prices.rend()); // Descending for bids
    }
    
    return prices;
}


