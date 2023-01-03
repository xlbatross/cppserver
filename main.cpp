#include <iostream>
#include <thread>
#include <pthread.h>
#include "tcpserver.h"
void clntHander(TCPServer * server, SOCKET clntSock);

int main(int, char**) {
    std::vector<std::thread> threadPool;
    TCPServer * server = new TCPServer();
    server->start();

    while (true)
    {
        std::cout << "waiting for clients.." << std::endl;
        SOCKET clntSock = server->acceptClient();
        std::cout << clntSock << std::endl;
        std::thread clntThread(clntHander, server, clntSock);
        threadPool.push_back(clntThread);
    }
}

void clntHander(TCPServer * server, SOCKET clntSock)
{
    while (true)
    {
        Encode * ecd = NULL;
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
          ecd = new EncodeChat(chat.Msg());
          server->sendData(clntSock, ecd); 
        } break;
        }

        delete dcd;
        if (ecd != NULL)
            delete ecd;
    }
}
