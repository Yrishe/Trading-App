#pragma once

#include <string>
#include <map>
#include "Wallet.h"

class User
{
public:
    User();
    User(const std::string& username, const std::string& email);
    
    // Getters
    const std::string& getUsername() const { return username; }
    const std::string& getEmail() const { return email; }
    const std::string& getPasswordHash() const { return passwordHash; }
    const std::string& getSalt() const { return salt; }
    int getPoints() const { return points; }
    double getTotalProfit() const { return totalProfit; }
    int getSuccessfulTrades() const { return successfulTrades; }
    int getTotalTrades() const { return totalTrades; }
    const std::string& getCreatedAt() const { return createdAt; }
    const std::string& getLastLogin() const { return lastLogin; }
    Wallet& getWallet() { return wallet; }
    const Wallet& getWallet() const { return wallet; }
    
    // Setters
    void setPasswordHash(const std::string& hash) { passwordHash = hash; }
    void setSalt(const std::string& s) { salt = s; }
    void setLastLogin(const std::string& timestamp) { lastLogin = timestamp; }
    
    // Points and trading statistics
    void addPoints(int pointsToAdd);
    void deductPoints(int pointsToDeduct);
    void recordTrade(bool successful, double profit);
    
    // Serialization
    std::string toJson() const;
    static User fromJson(const std::string& json);
    
    // Validation
    bool isValid() const;
    
private:
    std::string username;
    std::string email;
    std::string passwordHash;
    std::string salt;
    int points;
    double totalProfit;
    int successfulTrades;
    int totalTrades;
    std::string createdAt;
    std::string lastLogin;
    Wallet wallet;
    
    void initializeDefaults();
    std::string getCurrentTimestamp() const;
};