#include "Encryption.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <iostream>
#include <memory>
#include <cstring>

std::string Encryption::encrypt(const std::string& plaintext, const std::string& password)
{
    // Generate random salt and IV
    unsigned char salt[8];
    unsigned char iv[16];
    
    if (RAND_bytes(salt, sizeof(salt)) != 1 || RAND_bytes(iv, sizeof(iv)) != 1) {
        throw std::runtime_error("Failed to generate random bytes");
    }
    
    // Derive key from password
    unsigned char key[32];
    if (PKCS5_PBKDF2_HMAC(password.c_str(), password.length(), salt, sizeof(salt), 10000, EVP_sha256(), sizeof(key), key) != 1) {
        throw std::runtime_error("Failed to derive key");
    }
    
    // Create cipher context
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        throw std::runtime_error("Failed to create cipher context");
    }
    
    // Initialize encryption
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize encryption");
    }
    
    // Encrypt data
    std::vector<unsigned char> ciphertext(plaintext.length() + EVP_CIPHER_block_size(EVP_aes_256_cbc()));
    int len;
    int ciphertext_len;
    
    if (EVP_EncryptUpdate(ctx, ciphertext.data(), &len, reinterpret_cast<const unsigned char*>(plaintext.c_str()), plaintext.length()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to encrypt data");
    }
    ciphertext_len = len;
    
    if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to finalize encryption");
    }
    ciphertext_len += len;
    
    EVP_CIPHER_CTX_free(ctx);
    
    // Combine salt + iv + ciphertext
    std::vector<unsigned char> result;
    result.insert(result.end(), salt, salt + sizeof(salt));
    result.insert(result.end(), iv, iv + sizeof(iv));
    result.insert(result.end(), ciphertext.begin(), ciphertext.begin() + ciphertext_len);
    
    return base64Encode(result);
}

std::string Encryption::decrypt(const std::string& ciphertext, const std::string& password)
{
    // Decode base64
    std::vector<unsigned char> data = base64Decode(ciphertext);
    
    if (data.size() < 24) { // 8 bytes salt + 16 bytes IV
        throw std::runtime_error("Invalid ciphertext format");
    }
    
    // Extract salt, IV, and ciphertext
    unsigned char salt[8];
    unsigned char iv[16];
    std::memcpy(salt, data.data(), 8);
    std::memcpy(iv, data.data() + 8, 16);
    
    std::vector<unsigned char> encrypted_data(data.begin() + 24, data.end());
    
    // Derive key from password
    unsigned char key[32];
    if (PKCS5_PBKDF2_HMAC(password.c_str(), password.length(), salt, sizeof(salt), 10000, EVP_sha256(), sizeof(key), key) != 1) {
        throw std::runtime_error("Failed to derive key");
    }
    
    // Create cipher context
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        throw std::runtime_error("Failed to create cipher context");
    }
    
    // Initialize decryption
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize decryption");
    }
    
    // Decrypt data
    std::vector<unsigned char> plaintext(encrypted_data.size() + EVP_CIPHER_block_size(EVP_aes_256_cbc()));
    int len;
    int plaintext_len;
    
    if (EVP_DecryptUpdate(ctx, plaintext.data(), &len, encrypted_data.data(), encrypted_data.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to decrypt data");
    }
    plaintext_len = len;
    
    if (EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to finalize decryption");
    }
    plaintext_len += len;
    
    EVP_CIPHER_CTX_free(ctx);
    
    return std::string(reinterpret_cast<char*>(plaintext.data()), plaintext_len);
}

std::string Encryption::generateSalt(int length)
{
    std::vector<unsigned char> salt(length);
    if (RAND_bytes(salt.data(), length) != 1) {
        throw std::runtime_error("Failed to generate salt");
    }
    
    std::string result;
    for (unsigned char byte : salt) {
        char hex[3];
        sprintf(hex, "%02x", byte);
        result += hex;
    }
    
    return result;
}

std::string Encryption::hashPassword(const std::string& password, const std::string& salt)
{
    unsigned char hash[32];
    
    // Convert hex salt to bytes
    std::vector<unsigned char> salt_bytes;
    for (size_t i = 0; i < salt.length(); i += 2) {
        std::string byte_string = salt.substr(i, 2);
        unsigned char byte = static_cast<unsigned char>(strtol(byte_string.c_str(), nullptr, 16));
        salt_bytes.push_back(byte);
    }
    
    if (PKCS5_PBKDF2_HMAC(password.c_str(), password.length(), salt_bytes.data(), salt_bytes.size(), 10000, EVP_sha256(), sizeof(hash), hash) != 1) {
        throw std::runtime_error("Failed to hash password");
    }
    
    std::string result;
    for (int i = 0; i < 32; ++i) {
        char hex[3];
        sprintf(hex, "%02x", hash[i]);
        result += hex;
    }
    
    return result;
}

bool Encryption::verifyPassword(const std::string& password, const std::string& hash, const std::string& salt)
{
    std::cerr << "Debug: Computing hash for verification" << std::endl;
    std::cerr << "Debug: Input salt: " << salt << std::endl;
    std::cerr << "Debug: Expected hash: " << hash << std::endl;
    
    std::string computed_hash = hashPassword(password, salt);
    std::cerr << "Debug: Computed hash: " << computed_hash << std::endl;
    
    bool result = computed_hash == hash;
    std::cerr << "Debug: Hash comparison result: " << (result ? "MATCH" : "NO MATCH") << std::endl;
    
    return result;
}

std::string Encryption::base64Encode(const std::vector<unsigned char>& data)
{
    BIO* bio = BIO_new(BIO_s_mem());
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bio = BIO_push(b64, bio);
    
    BIO_write(bio, data.data(), data.size());
    BIO_flush(bio);
    
    BUF_MEM* buffer_ptr;
    BIO_get_mem_ptr(bio, &buffer_ptr);
    
    std::string result(buffer_ptr->data, buffer_ptr->length);
    BIO_free_all(bio);
    
    return result;
}

std::vector<unsigned char> Encryption::base64Decode(const std::string& encoded)
{
    BIO* bio = BIO_new_mem_buf(encoded.c_str(), encoded.length());
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bio = BIO_push(b64, bio);
    
    std::vector<unsigned char> result(encoded.length());
    int decoded_length = BIO_read(bio, result.data(), encoded.length());
    
    BIO_free_all(bio);
    
    result.resize(decoded_length);
    return result;
}