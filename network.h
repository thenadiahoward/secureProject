#ifndef _NETWORKING
#define _NETWORKING

#include <WinSock2.h>
#include <iostream>
#include <exception>

namespace network{
    const static std::wstring SELF = L"127.0.0.1";
    const int SERVER_PORT = 60800;
    const int BACKUP_SERVER_PORT = 60801;
    const WORD WINSOCK_VERSION_NEEDED = WINSOCK_VERSION;
    const int MAX_PARTICIPTANTS = 2;

    class server{
        WSADATA requiredData;
        sockaddr_in socketAddr;
        SOCKET serverSocket = INVALID_SOCKET;
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
            server(std::wstring );
            server(const server&);
            ~server();
            void cleanup();
            SOCKET acceptConnection();
    };

    class client{
        WSADATA requiredData;
        sockaddr socketAddr;
        SOCKET clientSocket = INVALID_SOCKET;
        /*
        1) init WSA DLL - WSAStartup()
        2) create a socket - socket()
        3) connect to server - connect()
        4) Send and receive data - recv(),send(),recvfrom(),sendto()
        5) Disconnect - closesocket()
        */
    };
};
#endif