#include "Wallet.h"
#include <iostream> 
#include "CSVReader.h"

Wallet::Wallet()
{
    
}
/**Insert currency to the wallet**/
void Wallet::insertCurrency(std::string type, double amount)
{
    double balance; //balance of the wallet
    if(amount < 0) 
    {
        throw std::exception{}; //throw an exception if the value is not valid
    }
    if(currencies.count(type) == 0) //count is counting elements with a specific key
    {
        balance = 0; //set the balance to 0 if the key is not found
    }
    else
    {
        balance = currencies[type]; //set the value contained in the key to balance
    }
    balance += amount; //update the balance 
    currencies[type] = balance; //update the key value in the wallet
}

/**Check if the wallet contain this much currency or more**/
bool Wallet::containsCurrency(std::string type, double amount)
{
    if(currencies.count(type) == 0) //count is counting elements with a specific key
        return false;
    else 
        return currencies[type] >= amount; //return the actual value in the wallet
}

std::string Wallet::toString() const
{   
    // this represent the string after concatenation
    std::string s;
    // represent one of the items in the map
    for(std::pair<std::string, double> pair : currencies)
    {
        // this represent the key or currency name
        std::string currency = pair.first;
        // this represent the value or amount
        double amount = pair.second;
        // std::to_string convert double to string
        s += currency + " : " + std::to_string(amount) + "\n";
    }
    return s;
};

bool Wallet::removeCurrency(std::string type, double amount)
{
    if(amount < 0) //if wallet is in negative value
    {
        return "You are in debt, please pay what you own.";
    }
    if(currencies.count(type) == 0) //if wallet is empty
    {
        return "Work harder! You don't have money.";
    }
    else //is there - do we have enough
    {
        if(containsCurrency(type,amount)) //we have enough
        {
            currencies[type] -= amount;
            return true;
        }
        else //they have currency but not enough
            return "You don't have enough money.";
    }
}

bool Wallet::canFulfillOrder(OrderBookEntry order)
{
    // currs represent current currency, representation = currs [BTC/ETH/...]
    std::vector<std::string> currs = CSVReader::tokenise(order.product, '/');
    //ask
    if(order.orderType == OrderBookType::ask)
    {
        double amount = order.amount;
        // currs[0] = C1
        std::string currency = currs[0];
        // used to check behaviour
        std::cout << "Wallet::canFulfillOrder " << currency << " : " << amount << std::endl;
        return containsCurrency(currency, amount);
    }

    // bid
    if(order.orderType == OrderBookType::bid)
    {
        double amount = order.amount * order.price;
        std::string currency = currs[1];
        // used to check behaviour
        std::cout << "Wallet::canFulfillOrder " << currency << " : " << amount << std::endl;
        return containsCurrency(currency, amount);
    }
    return false;
}


void Wallet::processSale(OrderBookEntry& sale)
{
    std::vector<std::string> currs = CSVReader::tokenise(sale.product, '/');
    // ask
    if (sale.orderType == OrderBookType::asksale)
    {
        double outgoingAmount = sale.amount;
        std::string outgoingCurrency = currs[0];
        double incomingAmount = sale.amount * sale.price;
        std::string incomingCurrency = currs[1];

        currencies[incomingCurrency] += incomingAmount;
        currencies[outgoingCurrency] -= outgoingAmount;

    }
    // bid
    if (sale.orderType == OrderBookType::bidsale)
    {
        double incomingAmount = sale.amount;
        std::string incomingCurrency = currs[0];
        double outgoingAmount = sale.amount * sale.price;
        std::string outgoingCurrency = currs[1];

        currencies[incomingCurrency] += incomingAmount;
        currencies[outgoingCurrency] -= outgoingAmount;
    }
}