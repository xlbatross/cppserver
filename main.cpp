#include <iostream>
#include <thread>
// #include "wtcpserver.h"
#include "ltcpserver.h"
// void clntHander(WTCPServer * server, SOCKET clntSock);
void clntHander(LTCPServer * server, int clntSock);

int main(int, char**) {
    // WTCPServer * server = new WTCPServer();
    LTCPServer * server = new LTCPServer();
    server->start();
    while (true)
    {
        std::cout << "waiting for clients.." << std::endl;
        // SOCKET clntSock = server->acceptClient();
        int clntSock = server->acceptClient();
        std::thread clntThread(clntHander, server, clntSock);
        clntThread.detach();
    }
    
    delete server;

    return 0;
}

// void clntHander(WTCPServer * server, SOCKET clntSock)
void clntHander(LTCPServer * server, int clntSock)
{
    while (true)
    {
        Decode * dcd = NULL;
        Encode * ecd = NULL;
        server->receiveData(clntSock, dcd);

        if (dcd == NULL)
            break;
        
        server->processData(clntSock, dcd, ecd);
        if (ecd != NULL)
        {
            server->sendData(clntSock, ecd);
            delete ecd;
        }
        
        delete dcd;
    }
}
