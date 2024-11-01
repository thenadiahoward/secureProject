#include <vector>
#include "networking/network.h"
#ifdef _OPENMP
#include <omp.h>
#endif

void acceptConnections(network::server&,std::vector<network::server>&);

int main(int argc,char** argv){
    omp_set_num_teams(network::MAX_PARTICIPTANTS);
    int num_threads;
    std::vector<network::server> connections = std::vector<network::server>(); //has race conditions
    std::string connectionIP = network::SELF;
    std::string temp;
    #ifdef _OPENMP
    num_threads = network::MAX_PARTICIPTANTS;
    #else
    num_threads = 1;
    #endif
    std::cout << "Please enter IP for local server (blank for self): ";
    std::getline(std::cin,temp);
    if (temp != "") connectionIP = temp;
    network::server testServer = network::server(connectionIP);
    temp = "";
    int threadID;
    
    #pragma omp parallel num_threads(num_threads) private(connectionIP,temp,threadID)
    {
        connectionIP = network::SELF;
        threadID = omp_get_thread_num();

        if(threadID == 0){ //if master thread, aka the server or 1 thread
            std::cout << "Hello\n";
            for(;;){ //main application loop
                if(connections.size() < (network::MAX_PARTICIPTANTS - 1)) acceptConnections(testServer,connections);

            }
        }else{
            std::cout << "Please enter the connection string (blank for self): ";
            std::getline(std::cin,temp);
            if (temp != "") connectionIP = temp;
            else std::cout << "IP Empty, connecting to loopback\n";
            network::client localClient = network::client(connectionIP);
        }
    }
}

void acceptConnections(network::server& server,std::vector<network::server>& connections){
    network::server temp = network::server(server.acceptConnection());
    #pragma omp critical //so two things don't get pushed at the same time and both end up frankenstein'd
    connections.push_back(temp);
}