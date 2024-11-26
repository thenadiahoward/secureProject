#ifndef _NETWORKING
#define _NETWORKING
#define ENCRYPTION_KEY "ThisIsAnEncryptionKey123456789" // Replace with securely generated key
#define ENCRYPTION_IV "YourInitVector1234"         // Replace with securely generated IV

#include <WinSock2.h>
#include <iostream>
#include <exception>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/rand.h>

namespace network{
    const static std::string SELF = "127.0.0.1";
    const int SERVER_PORT = 60800;
    const int BACKUP_SERVER_PORT = 60801;
    const WORD WINSOCK_VERSION_NEEDED = WINSOCK_VERSION;
    const int MAX_PARTICIPTANTS = 2; //minimum of 2 
    const int BUFFER_SIZE = 1024;
    const int MAX_MESSAGE_LEN = BUFFER_SIZE;

    std::string encryptMessage(const std::string& plaintext, const std::string& key, const std::string& iv) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) throw std::runtime_error("Failed to create encryption context");

    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, (unsigned char*)key.c_str(), (unsigned char*)iv.c_str())) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize encryption");
    }

    std::string ciphertext(plaintext.size() + EVP_CIPHER_block_size(EVP_aes_256_cbc()), '\0');
    int len = 0, ciphertext_len = 0;

    if (1 != EVP_EncryptUpdate(ctx, (unsigned char*)ciphertext.data(), &len, (unsigned char*)plaintext.c_str(), plaintext.size())) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Encryption failed");
    }
    ciphertext_len += len;

    if (1 != EVP_EncryptFinal_ex(ctx, (unsigned char*)ciphertext.data() + ciphertext_len, &len)) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Final encryption step failed");
    }
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    ciphertext.resize(ciphertext_len);
    return ciphertext;
}

std::string decryptMessage(const std::string& ciphertext, const std::string& key, const std::string& iv) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) throw std::runtime_error("Failed to create decryption context");

    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, (unsigned char*)key.c_str(), (unsigned char*)iv.c_str())) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize decryption");
    }

    std::string plaintext(ciphertext.size(), '\0');
    int len = 0, plaintext_len = 0;

    if (1 != EVP_DecryptUpdate(ctx, (unsigned char*)plaintext.data(), &len, (unsigned char*)ciphertext.c_str(), ciphertext.size())) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Decryption failed");
    }
    plaintext_len += len;

    if (1 != EVP_DecryptFinal_ex(ctx, (unsigned char*)plaintext.data() + plaintext_len, &len)) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Final decryption step failed");
    }
    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    plaintext.resize(plaintext_len);
    return plaintext;
}


    class server{
        sockaddr_in socketAddr;
        SOCKET serverSocket = INVALID_SOCKET;
        bool mainServer = true;

        void cleanup() const;
        /*
        1) Init WSA DLL - WSAStartup()
        2) create a socket - socket()
        3) bind the socket - bind()
        4) Listen on the socket - listen()
        5) Accept a connection - accept(),connect()
        6) send and receive data - rect(),send(),recvfrom(),sendto()
        7) Disconnect - closesocket()
        */
        public:
            WSADATA requiredData;

            server(std::string );
            server(const server&);
            server();
            server(SOCKET);
            server(WSAData* );
            ~server();

            SOCKET acceptConnection() const;
            std::string receiveMessage() const;
            void putIntoListen() const;
            int startWSA(WSAData* );
    };

    class client{
        sockaddr_in socketAddr;
        SOCKET clientSocket = INVALID_SOCKET;

        void cleanup();
        /*
        1) init WSA DLL - WSAStartup()
        2) create a socket - socket()
        3) connect to server - connect()
        4) Send and receive data - recv(),send(),recvfrom(),sendto()
        5) Disconnect - closesocket()
        */
        public:
            WSADATA requiredData;

            client(std::string );
            client(std::string, WSAData* );
            client(WSAData* );
            client(const client& );
            client();
            ~client();
            bool sendMessage(std::string);
            bool sendFile(/*file goes here*/);
    };
};

#endif