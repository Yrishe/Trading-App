#include "User.h"
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <algorithm>

User::User()
{
    initializeDefaults();
}

User::User(const std::string& username, const std::string& email)
    : username(username), email(email)
{
    initializeDefaults();
}

void User::initializeDefaults()
{
    points = 1000; // Starting points
    totalProfit = 0.0;
    successfulTrades = 0;
    totalTrades = 0;
    createdAt = getCurrentTimestamp();
    lastLogin = getCurrentTimestamp();
    
    // Initialize wallet with some starting currencies
    wallet.insertCurrency("BTC", 0.1);
    wallet.insertCurrency("ETH", 1.0);
    wallet.insertCurrency("USDT", 1000.0);
    wallet.insertCurrency("DOGE", 100.0);
}

void User::addPoints(int pointsToAdd)
{
    if (pointsToAdd > 0) {
        points += pointsToAdd;
    }
}

void User::deductPoints(int pointsToDeduct)
{
    if (pointsToDeduct > 0 && points >= pointsToDeduct) {
        points -= pointsToDeduct;
    }
}

void User::recordTrade(bool successful, double profit)
{
    totalTrades++;
    if (successful) {
        successfulTrades++;
        totalProfit += profit;
        
        // Award points based on profit
        int pointsEarned = static_cast<int>(profit * 10); // 10 points per unit profit
        if (pointsEarned > 0) {
            addPoints(pointsEarned);
        }
    } else {
        totalProfit += profit; // profit will be negative for losses
        
        // Deduct points for losses (but not below 0)
        int pointsLost = static_cast<int>(std::abs(profit) * 5); // 5 points per unit loss
        deductPoints(pointsLost);
    }
}

std::string User::toJson() const
{
    std::ostringstream json;
    json << "{";
    json << "\"username\":\"" << username << "\",";
    json << "\"email\":\"" << email << "\",";
    json << "\"passwordHash\":\"" << passwordHash << "\",";
    json << "\"salt\":\"" << salt << "\",";
    json << "\"points\":" << points << ",";
    json << "\"totalProfit\":" << std::fixed << std::setprecision(2) << totalProfit << ",";
    json << "\"successfulTrades\":" << successfulTrades << ",";
    json << "\"totalTrades\":" << totalTrades << ",";
    json << "\"createdAt\":\"" << createdAt << "\",";
    json << "\"lastLogin\":\"" << lastLogin << "\",";
    json << "\"wallet\":\"" << wallet.toString() << "\"";
    json << "}";
    return json.str();
}

User User::fromJson(const std::string& json)
{
    User user;
    
    // Simple JSON parsing (in a real application, use a proper JSON library)
    size_t pos = 0;
    
    // Extract username
    pos = json.find("\"username\":\"") + 12;
    size_t end = json.find("\"", pos);
    user.username = json.substr(pos, end - pos);
    
    // Extract email
    pos = json.find("\"email\":\"") + 9;
    end = json.find("\"", pos);
    user.email = json.substr(pos, end - pos);
    
    // Extract passwordHash
    pos = json.find("\"passwordHash\":\"") + 16;
    end = json.find("\"", pos);
    user.passwordHash = json.substr(pos, end - pos);
    
    // Extract salt
    pos = json.find("\"salt\":\"") + 8;
    end = json.find("\"", pos);
    user.salt = json.substr(pos, end - pos);
    
    // Extract points
    pos = json.find("\"points\":") + 9;
    end = json.find(",", pos);
    std::string pointsStr = json.substr(pos, end - pos);
    // std::cerr << "Debug: Parsing points: '" << pointsStr << "'" << std::endl;
    user.points = std::stoi(pointsStr);
    
    // Extract totalProfit
    pos = json.find("\"totalProfit\":") + 14;
    end = json.find(",", pos);
    user.totalProfit = std::stod(json.substr(pos, end - pos));
    
    // Extract successfulTrades
    pos = json.find("\"successfulTrades\":") + 19;
    end = json.find(",", pos);
    std::string successfulTradesStr = json.substr(pos, end - pos);
    // std::cerr << "Debug: Parsing successfulTrades: '" << successfulTradesStr << "'" << std::endl;
    user.successfulTrades = std::stoi(successfulTradesStr);
    
    // Extract totalTrades - find the value between : and ,
    size_t totalTradesPos = json.find("\"totalTrades\":");
    pos = totalTradesPos + 14; // "totalTrades": is 14 characters
    // Skip any whitespace after the colon
    while (pos < json.length() && (json[pos] == ' ' || json[pos] == '\t' || json[pos] == '\n' || json[pos] == '\r')) {
        pos++;
    }
    // Find the next comma that's not inside quotes
    end = pos;
    while (end < json.length() && json[end] != ',') {
        end++;
    }
    std::string totalTradesStr = json.substr(pos, end - pos);
    // std::cerr << "Debug: Parsing totalTrades: '" << totalTradesStr << "'" << std::endl;
    user.totalTrades = std::stoi(totalTradesStr);
    
    // Extract createdAt
    pos = json.find("\"createdAt\":\"") + 13;
    end = json.find("\"", pos);
    user.createdAt = json.substr(pos, end - pos);
    
    // Extract lastLogin
    pos = json.find("\"lastLogin\":\"") + 13;
    end = json.find("\"", pos);
    user.lastLogin = json.substr(pos, end - pos);
    
    // Skip wallet parsing for now since it contains multiline data
    // that breaks the simple JSON parsing
    
    return user;
}

bool User::isValid() const
{
    return !username.empty() && !email.empty() && !passwordHash.empty() && !salt.empty();
}

std::string User::getCurrentTimestamp() const
{
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}