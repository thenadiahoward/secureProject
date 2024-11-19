#include <vector>
#include "networking/network.h"

/*IMPORTANT: FOR TESTING ONLY*/

int main(int argc, char** argv){
    std::vector<network::server> connections = std::vector<network::server>(); //has race conditions
    std::string ipConnect;
    if(argc - 1){ 
        ipConnect = argv[1];
    }else{
        ipConnect = "172.20.208.1";
    }
    network::server testServer = network::server(ipConnect);
    network::server temp = network::server(testServer.acceptConnection());
    connections.push_back(temp);
    std::string receivedMessage;
    while(connections.size()){
        receivedMessage = connections.at(0).receiveMessage();
        if(receivedMessage != ""){
            std::cout << connections.at(0).receiveMessage() << "\n";
        }else{
            connections.erase(connections.begin());
        }
    }
}