#include <vector>
#include "networking/network.h"

/*IMPORTANT: FOR TESTING ONLY*/

int main(int argc, char** argv){
    std::string ipConnect;
    if(argc - 1){
        ipConnect = argv[1];
    }else{
        ipConnect = "127.0.0.1";
    }
    network::client testClient = network::client(ipConnect);
    testClient.sendMessage("Hello from a socket\n");
}