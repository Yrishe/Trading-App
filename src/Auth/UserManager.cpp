#include "UserManager.h"
#include "Encryption.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <regex>
#include <iostream>
#include <chrono>
#include <sstream>
#include <iomanip>

UserManager::UserManager()
{
    // Set database path to application data directory
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(appDataPath);
    databasePath = appDataPath.toStdString() + "/trading_app.db";
}

UserManager::~UserManager()
{
    if (QSqlDatabase::contains("qt_sql_default_connection")) {
        QSqlDatabase::database().close();
    }
}

bool UserManager::initialize()
{
    // Initialize SQLite database
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QString::fromStdString(databasePath));
    
    if (!db.open()) {
        std::cerr << "Error opening database: " << db.lastError().text().toStdString() << std::endl;
        return false;
    }
    
    return createTables();
}

bool UserManager::createTables()
{
    QSqlQuery query;
    
    // Create users table
    QString createUsersTable = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT UNIQUE NOT NULL,
            email TEXT UNIQUE NOT NULL,
            password_hash TEXT NOT NULL,
            salt TEXT NOT NULL,
            encrypted_data TEXT NOT NULL,
            created_at TEXT NOT NULL,
            last_login TEXT
        )
    )";
    
    if (!query.exec(createUsersTable)) {
        std::cerr << "Error creating users table: " << query.lastError().text().toStdString() << std::endl;
        return false;
    }
    
    return true;
}

bool UserManager::registerUser(const std::string& username, const std::string& email, const std::string& password)
{
    // Validate input
    if (username.empty() || email.empty() || password.empty()) {
        return false;
    }
    
    if (!isValidEmail(email) || !isValidPassword(password)) {
        return false;
    }
    
    if (userExists(username) || emailExists(email)) {
        return false;
    }
    
    try {
        // Create new user
        User newUser(username, email);
        
        // Generate salt and hash password
        std::string salt = Encryption::generateSalt(); // Use default 32 bytes = 64 hex chars
        std::cerr << "Debug: Generated salt during registration: " << salt << std::endl;
        std::string passwordHash = Encryption::hashPassword(password, salt);
        std::cerr << "Debug: Generated hash during registration: " << passwordHash << std::endl;
        
        newUser.setSalt(salt);
        newUser.setPasswordHash(passwordHash);
        
        // Encrypt user data
        std::string userData = newUser.toJson();
        std::string encryptedData = Encryption::encrypt(userData, password);
        
        // Save to database
        QSqlQuery query;
        query.prepare(R"(
            INSERT INTO users (username, email, password_hash, salt, encrypted_data, created_at)
            VALUES (?, ?, ?, ?, ?, ?)
        )");
        
        query.addBindValue(QString::fromStdString(username));
        query.addBindValue(QString::fromStdString(email));
        query.addBindValue(QString::fromStdString(passwordHash));
        query.addBindValue(QString::fromStdString(salt));
        query.addBindValue(QString::fromStdString(encryptedData));
        query.addBindValue(QString::fromStdString(newUser.getCreatedAt()));
        
        if (!query.exec()) {
            std::cerr << "Error inserting user: " << query.lastError().text().toStdString() << std::endl;
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error registering user: " << e.what() << std::endl;
        return false;
    }
}

std::shared_ptr<User> UserManager::authenticateUser(const std::string& username, const std::string& password)
{
    try {
        QSqlQuery query;
        query.prepare("SELECT password_hash, salt, encrypted_data FROM users WHERE username = ? OR email = ?");
        query.addBindValue(QString::fromStdString(username));
        query.addBindValue(QString::fromStdString(username));
        
        if (!query.exec()) {
            std::cerr << "Database query error: " << query.lastError().text().toStdString() << std::endl;
            return nullptr;
        }
        
        if (!query.next()) {
            std::cerr << "User not found: " << username << std::endl;
            return nullptr;
        }
        
        std::string storedHash = query.value(0).toString().toStdString();
        std::string salt = query.value(1).toString().toStdString();
        std::string encryptedData = query.value(2).toString().toStdString();
        
        // Verify password
        std::cerr << "Debug: Attempting password verification" << std::endl;
        std::cerr << "Debug: Stored hash length: " << storedHash.length() << std::endl;
        std::cerr << "Debug: Salt length: " << salt.length() << std::endl;
        std::cerr << "Debug: Password length: " << password.length() << std::endl;
        
        if (!Encryption::verifyPassword(password, storedHash, salt)) {
            std::cerr << "Debug: Password verification failed" << std::endl;
            return nullptr;
        }
        
        std::cerr << "Debug: Password verification successful" << std::endl;
        
        // Decrypt user data
        std::string userData = Encryption::decrypt(encryptedData, password);
        // std::cerr << "Debug: Decrypted user data: " << userData << std::endl;
        
        // Create user object
        std::shared_ptr<User> user;
        try {
            user = std::make_shared<User>(User::fromJson(userData));
        } catch (const std::invalid_argument& e) {
            // std::cerr << "Debug: JSON parsing failed with invalid_argument: " << e.what() << std::endl;
            return nullptr;
        } catch (const std::out_of_range& e) {
            // std::cerr << "Debug: JSON parsing failed with out_of_range: " << e.what() << std::endl;
            return nullptr;
        }
        
        // Update last login
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        user->setLastLogin(ss.str());
        saveUser(*user);
        
        currentUser = user;
        return user;
        
    } catch (const std::exception& e) {
        std::cerr << "Error authenticating user: " << e.what() << std::endl;
        return nullptr;
    }
}

bool UserManager::saveUser(const User& user)
{
    if (!currentUser) {
        return false;
    }
    
    try {
        // We need the password to encrypt data, but we don't store it
        // For now, we'll update non-encrypted fields only
        QSqlQuery query;
        query.prepare("UPDATE users SET last_login = ? WHERE username = ?");
        query.addBindValue(QString::fromStdString(user.getLastLogin()));
        query.addBindValue(QString::fromStdString(user.getUsername()));
        
        if (!query.exec()) {
            std::cerr << "Error updating user: " << query.lastError().text().toStdString() << std::endl;
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error saving user: " << e.what() << std::endl;
        return false;
    }
}

std::shared_ptr<User> UserManager::loadUser(const std::string& username)
{
    // This would require the password to decrypt, so it's mainly used internally
    return nullptr;
}

bool UserManager::userExists(const std::string& username)
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM users WHERE username = ?");
    query.addBindValue(QString::fromStdString(username));
    
    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    
    return false;
}

bool UserManager::emailExists(const std::string& email)
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM users WHERE email = ?");
    query.addBindValue(QString::fromStdString(email));
    
    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    
    return false;
}

void UserManager::logout()
{
    currentUser.reset();
}

bool UserManager::isValidEmail(const std::string& email)
{
    const std::regex emailPattern(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    return std::regex_match(email, emailPattern);
}

bool UserManager::isValidPassword(const std::string& password)
{
    // Password requirements: at least 8 characters, contains uppercase, lowercase, digit
    if (password.length() < 8) {
        return false;
    }
    
    bool hasUpper = false, hasLower = false, hasDigit = false;
    
    for (char c : password) {
        if (std::isupper(c)) hasUpper = true;
        else if (std::islower(c)) hasLower = true;
        else if (std::isdigit(c)) hasDigit = true;
    }
    
    return hasUpper && hasLower && hasDigit;
}