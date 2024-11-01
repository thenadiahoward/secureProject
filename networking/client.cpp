#include "network.h"

#include <WS2tcpip.h>
#include <codecvt>
#include <locale>

using namespace network;
using convert_type = std::codecvt_utf8<wchar_t>;

std::wstring_convert<convert_type, wchar_t> converter;

client::client(std::string ipAddr = SELF){
    //Step 1
    std::cout << "Starting WSA\n";
    int WSAError = WSAStartup(WINSOCK_VERSION_NEEDED,&requiredData);
    if(WSAError){
        std::cerr << "Winsock dll not found\n";
        throw std::runtime_error("Winsock dll not found");
    }else{
        std::cout << "Status: " << requiredData.szSystemStatus << std::endl; //print WSA status and flush
    }

    //step 2
    std::cout << "Creating socket\n";
    clientSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(clientSocket==INVALID_SOCKET){
        cleanup();
        std::cerr << "Error creating socket: " << WSAGetLastError() << std::endl;
        throw std::runtime_error("Error creating socket: " + WSAGetLastError());
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
            std::cerr << "\nConnecting failed(" << error <<")\nIP: " << ipAddr << ":"<< SERVER_PORT <<"\nAttempting backup port\n";
            socketAddr.sin_port = htons(BACKUP_SERVER_PORT);
            goto retry;
        }
        cleanup();
        std::cerr << "Error connecting to " << ipAddr << ":" << BACKUP_SERVER_PORT << " with error: " << error << std::endl;
        throw std::runtime_error("Error connecting to server: " + error);
    }
    std::cout << "Connection established\n";
}



void client::cleanup(){
    if(clientSocket != INVALID_SOCKET) closesocket(this->clientSocket);
    int WSAError = WSACleanup();
    int error = WSAGetLastError();
    if(WSAError && error != 10093){
        std::cerr << "Cleanup error: "<< error <<"\n";
        throw std::runtime_error("Cleanup error: " + error);
    }else if(error == 10093){
        std::cerr << "WSA does not exist/already shutdown\n";
    }
}

client::~client(){
    cleanup();
}

bool client::sendMessage(std::string message){
    int error;
    if(send(clientSocket,message.c_str(),message.length(),0) == SOCKET_ERROR && (error = WSAGetLastError())){
        std::cerr << "Error sending message: " << error << std::endl;
        return false;
    }else{
        return true;
    }
}