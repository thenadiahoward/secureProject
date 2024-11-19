#include <vector>
#include "networking/network.h"

/*IMPORTANT: FOR TESTING ONLY*/

int main(int argc, char** argv){
    std::string ipConnect;
    if(argc - 1){
        ipConnect = argv[1];
    }else{
        ipConnect = "172.20.208.1";
    }
    network::client testClient = network::client(ipConnect);
    testClient.sendMessage("Hello from a socket\n");
    std::string inputMsg;
    while(true){
        std::cout << "Messaage: ";
        std::getline(std::cin,inputMsg);
        testClient.sendMessage(inputMsg);
        if(inputMsg == ""){
            break;
        }
    }
}