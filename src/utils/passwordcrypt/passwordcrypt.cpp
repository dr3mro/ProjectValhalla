#include "passwordcrypt.hpp"
#include "sodium/core.h"
#include "sodium/randombytes.h"
#include <cstring>
#include <iostream>
#include <sodium/crypto_pwhash_scryptsalsa208sha256.h>
PasswordCrypt::PasswordCrypt() {
    if (sodium_init() < 0) {
        std::cerr << "Failed to initialize libsodium" << std::endl;
    }
}

std::optional<std::string> PasswordCrypt::hashPassword(const std::string &password) {
    std::optional<std::string> hash;

    // Salt for the hashing (must be random)
    unsigned char salt[crypto_pwhash_scryptsalsa208sha256_SALTBYTES];
    randombytes_buf(salt, sizeof salt);

    // Buffer to store the hashed password
    unsigned char hashed_password[crypto_pwhash_scryptsalsa208sha256_STRBYTES];

    // Hash the password using the scrypt algorithm
    if (crypto_pwhash_scryptsalsa208sha256_str(
            (char *) hashed_password,
            password.c_str(),
            password.length(),
            crypto_pwhash_scryptsalsa208sha256_OPSLIMIT_MIN,
            crypto_pwhash_scryptsalsa208sha256_MEMLIMIT_MIN) != 0) {
        // out of memory
        std::cerr << "Failed to hash password" << std::endl;
        return std::nullopt;
    }

    hash = std::string(reinterpret_cast<char *>(hashed_password));
    return hash;
}
bool PasswordCrypt::verifyPassword(const std::string &password, const std::string &hash) const {
    // Verify the password using the scrypt algorithm
    if (crypto_pwhash_scryptsalsa208sha256_str_verify(hash.c_str(), password.c_str(), password.length()) != 0) {
        // Password does not match
        return false;
    }
    // Password matches
    return true;
}