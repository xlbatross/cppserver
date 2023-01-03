#include <iostream>
#include <thread>
// #include "wtcpserver.h"
#include "ltcpserver.h"
// void clntHander(WTCPServer * server, SOCKET clntSock);
void clntHander(LTCPServer * server, int clntSock);

int main(int, char**) {
    std::vector<std::thread> threadPool;
    // WTCPServer * server = new WTCPServer();
    LTCPServer * server = new LTCPServer();
    server->start();

    while (true)
    {
        std::cout << "waiting for clients.." << std::endl;
        // SOCKET clntSock = server->acceptClient();
        int clntSock = server->acceptClient();
        std::cout << clntSock << std::endl;
        std::thread clntThread(clntHander, server, clntSock);
        clntThread.detach();
    }
}

// void clntHander(WTCPServer * server, SOCKET clntSock)
void clntHander(LTCPServer * server, int clntSock)
{
    while (true)
    {
        Decode * dcd = NULL;
        server->receiveData(clntSock, dcd);

        if (dcd == NULL)
            break;

        switch(dcd->Type())
        {
        case Decode::Chat:
        {
          DecodeChat chat((DecodeTCP *)dcd);
          std::cout << "client : " << chat.Msg() << std::endl;
          EncodeChat ecd(chat.Msg());
          server->sendData(clntSock, &ecd); 
        } break;
        }

        delete dcd;
    }
}
