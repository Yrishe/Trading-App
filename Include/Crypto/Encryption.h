#pragma once

#include <string>
#include <vector>

class Encryption
{
public:
    /**
     * Encrypt a string using AES-256-CBC
     * @param plaintext The text to encrypt
     * @param password The password for encryption
     * @return Encrypted data as base64 string
     */
    static std::string encrypt(const std::string& plaintext, const std::string& password);
    
    /**
     * Decrypt a string using AES-256-CBC
     * @param ciphertext The encrypted text (base64)
     * @param password The password for decryption
     * @return Decrypted plaintext
     */
    static std::string decrypt(const std::string& ciphertext, const std::string& password);
    
    /**
     * Generate a secure random salt
     * @param length Length of the salt
     * @return Random salt as hex string
     */
    static std::string generateSalt(int length = 32);
    
    /**
     * Hash a password with salt using PBKDF2
     * @param password The password to hash
     * @param salt The salt to use
     * @return Hashed password
     */
    static std::string hashPassword(const std::string& password, const std::string& salt);
    
    /**
     * Verify a password against a hash
     * @param password The password to verify
     * @param hash The stored hash
     * @param salt The salt used
     * @return True if password matches
     */
    static bool verifyPassword(const std::string& password, const std::string& hash, const std::string& salt);
    
private:
    static std::string base64Encode(const std::vector<unsigned char>& data);
    static std::vector<unsigned char> base64Decode(const std::string& encoded);
};