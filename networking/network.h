#ifndef _NETWORKING
#define _NETWORKING

#include <WinSock2.h>
#include <iostream>
#include <exception>

namespace network{
    const static std::string SELF = "127.0.0.1";
    const int SERVER_PORT = 60800;
    const int BACKUP_SERVER_PORT = 60801;
    const WORD WINSOCK_VERSION_NEEDED = WINSOCK_VERSION;
    const int MAX_PARTICIPTANTS = 2; //minimum of 2 
    const int BUFFER_SIZE = 1024;
    const int MAX_MESSAGE_LEN = BUFFER_SIZE;

    class server{
        WSADATA requiredData;
        sockaddr_in socketAddr;
        SOCKET serverSocket = INVALID_SOCKET;
        char buffer[BUFFER_SIZE] = {0};

        void cleanup();
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
            server(std::string );
            server(const server&);
            server(SOCKET );
            ~server();
            SOCKET acceptConnection();
            std::string receiveMessage();
    };

    class client{
        WSADATA requiredData;
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
        client(std::string );
        client(const client& );
        ~client();
        bool sendMessage(std::string);
        bool sendFile(/*file goes here*/);

    };
};

#endif