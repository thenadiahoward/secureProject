#include "network.h"

size_t network::encryptMessage(const std::string& plaintext, unsigned char* output, const std::string& key, const std::string& iv) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) throw std::runtime_error("Failed to create encryption context");

    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, (unsigned char*)key.c_str(), (unsigned char*)iv.c_str())) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize encryption");
    }

    int len = 0, ciphertext_len = 0;
    if (1 != EVP_EncryptUpdate(ctx,output, &len, (unsigned char*)plaintext.c_str(), plaintext.size() + 1)) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Encryption failed");
    }
    ciphertext_len += len;

    if (1 != EVP_EncryptFinal_ex(ctx, output + ciphertext_len, &len)) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Final encryption step failed");
    }
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    return ciphertext_len;
}

std::string network::decryptMessage(const unsigned char* ciphertext, size_t ciphertext_size, const std::string& key, const std::string& iv) {
    bool fail = false;
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (ctx == nullptr) throw std::runtime_error("Failed to create decryption context");

    fail = !EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, (unsigned char*)key.c_str(), (unsigned char*)iv.c_str()); 
    if (fail) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize decryption");
    }

    int plaintext_len = ciphertext_size;
    int len = 0;
    std::string plaintext(plaintext_len, '\0');

    EVP_CIPHER_CTX_set_padding(ctx,0);

    fail = !EVP_DecryptUpdate(ctx, (unsigned char*)plaintext.data(), &plaintext_len, ciphertext, ciphertext_size);
    if (fail) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Decryption failed");
    }
    //plaintext_len += len;

    fail = !EVP_DecryptFinal_ex(ctx, ((unsigned char*)plaintext.data()) + plaintext_len, &len);
    if (fail) {    
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Final decryption step failed");
    }
    //plaintext_len += len;
    EVP_CIPHER_CTX_free(ctx);
    int trim_bytes = 0;
    for(auto reverse_iter = plaintext.rbegin();reverse_iter != plaintext.rend();reverse_iter++){ //check each chara from end to beginning
        if(*reverse_iter == '\0'){ //if we get a valid character
            break; //break out of loop
        }
        trim_bytes++;
    }
    plaintext.resize(plaintext.size() - trim_bytes);
    return plaintext;
}