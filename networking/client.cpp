#include "network.h"

#include <WS2tcpip.h>
#include <codecvt>
#include <locale>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/rand.h>

using namespace network;
using convert_type = std::codecvt_utf8<wchar_t>;

std::wstring_convert<convert_type, wchar_t> converter;


client::client(std::string ipAddr = SELF){
    //Step 1
    std::cout << "Starting WSA\n";
    int WSAError = WSAStartup(WINSOCK_VERSION_NEEDED,&requiredData);
    if(WSAError){
        std::cerr << "ERROR: Winsock dll not found\n";
        throw std::runtime_error("Winsock dll not found");
    }else{
        std::cout << "Status: " << requiredData.szSystemStatus << std::endl; //print WSA status and flush
    }

    //step 2
    std::cout << "Creating socket\n";
    clientSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(clientSocket==INVALID_SOCKET){
        cleanup();
        std::cerr << "ERROR: Error creating socket: " << WSAGetLastError() << std::endl;
        throw std::runtime_error("Error creating socket");
    }

    //step 3
    std::cout << "Connecting...";
    socketAddr.sin_port = htons(SERVER_PORT);
    retry:
    socketAddr.sin_family = AF_INET;
    InetPton(AF_INET,ipAddr.c_str(),&socketAddr.sin_addr.s_addr); //there's an eroneous error of "const char* is incompatiable with LPCWSTR", ignore it
    int error;
    if(connect(clientSocket,(SOCKADDR*)&socketAddr,sizeof(socketAddr)) == SOCKET_ERROR && (error = WSAGetLastError())){
        if(socketAddr.sin_port != htons(BACKUP_SERVER_PORT)){
            std::cerr << "\nWARNING: Connecting failed(" << error <<")\nIP: " << ipAddr << ":"<< SERVER_PORT <<"\nAttempting backup port\n";
            socketAddr.sin_port = htons(BACKUP_SERVER_PORT);
            goto retry;
        }
        cleanup();
        std::cerr << "ERROR: Error connecting to " << ipAddr << ":" << BACKUP_SERVER_PORT << " with error: " << error << std::endl;
        throw std::runtime_error("Error connecting to server");
    }
    std::cout << "Connection established\n";
}



void client::cleanup(){
    if(clientSocket != INVALID_SOCKET) closesocket(this->clientSocket);
    int WSAError = WSACleanup();
    int error = WSAGetLastError();
    if(WSAError && error != 10093){
        std::cerr << "ERROR: Cleanup error: "<< error <<"\n";
        throw std::runtime_error("Cleanup error");
    }else if(error == 10093){
        std::cerr << "WARNING: WSA does not exist/already shutdown\n";
    }
}

client::~client(){
    cleanup();
}

bool client::sendMessage(std::string message){
    std::string key = "ThisIsAnEncryptionKey123456789";
    std::string iv = "YourInitVector1234";
    std::string encryptedMessage = encryptMessage(message, key, iv); // encrypt message
    int error;
    if(send(clientSocket,encryptedMessage.c_str(),encryptedMessage.length(),0) == SOCKET_ERROR && (error = WSAGetLastError())){
        std::cerr << "WARNING: Error sending message: " << error << std::endl;
        return false;
    }else{
        return true;
    }
}

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