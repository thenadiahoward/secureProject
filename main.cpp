#include "network.h"

int main(int argc,char** argv){
    auto testServer = networking::server();
    testServer.acceptConnection();

}