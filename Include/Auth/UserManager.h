#pragma once

#include <string>
#include <memory>
#include <map>
#include "User.h"

class UserManager
{
public:
    UserManager();
    ~UserManager();
    
    /**
     * Initialize the user manager and create database if needed
     */
    bool initialize();
    
    /**
     * Register a new user
     * @param username Unique username
     * @param email User's email
     * @param password Plain text password (will be hashed)
     * @return True if registration successful
     */
    bool registerUser(const std::string& username, const std::string& email, const std::string& password);
    
    /**
     * Authenticate a user
     * @param username Username or email
     * @param password Plain text password
     * @return Pointer to User object if successful, nullptr otherwise
     */
    std::shared_ptr<User> authenticateUser(const std::string& username, const std::string& password);
    
    /**
     * Save user data to database
     * @param user User object to save
     * @return True if save successful
     */
    bool saveUser(const User& user);
    
    /**
     * Load user data from database
     * @param username Username to load
     * @return Pointer to User object if found, nullptr otherwise
     */
    std::shared_ptr<User> loadUser(const std::string& username);
    
    /**
     * Check if username exists
     * @param username Username to check
     * @return True if username exists
     */
    bool userExists(const std::string& username);
    
    /**
     * Check if email exists
     * @param email Email to check
     * @return True if email exists
     */
    bool emailExists(const std::string& email);
    
    /**
     * Get current logged in user
     * @return Pointer to current user or nullptr
     */
    std::shared_ptr<User> getCurrentUser() const { return currentUser; }
    
    /**
     * Logout current user
     */
    void logout();
    
    /**
     * Validate email format
     * @param email Email to validate
     * @return True if email format is valid
     */
    static bool isValidEmail(const std::string& email);
    
    /**
     * Validate password strength
     * @param password Password to validate
     * @return True if password meets requirements
     */
    static bool isValidPassword(const std::string& password);
    
private:
    std::shared_ptr<User> currentUser;
    std::string databasePath;
    
    bool createDatabase();
    bool createTables();
    std::string encryptUserData(const std::string& data, const std::string& password);
    std::string decryptUserData(const std::string& encryptedData, const std::string& password);
};