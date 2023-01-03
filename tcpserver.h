#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <winsock2.h>
#include <map>
#include <mutex>
#include <iostream>
#include "decode.h"
#include "encode.h"

class TCPServer
{
public:
    std::mutex m;

    TCPServer();
    ~TCPServer();

    void start(int port = 2500);
    SOCKET acceptClient();

    int receiveBytes(const SOCKET clntSock, char * & rawData);
    bool receiveData(const SOCKET clntSock, Decode * & dcd);

    int sendBytes(const SOCKET clntSock, const char *headerBytes, const int headerSize, const char *dataBytes, const int dataSize);
    bool sendData(const SOCKET clntSock, Encode * ecd);
    
private:
    WSADATA wsaData;

    SOCKET servSock;
    SOCKADDR_IN servAddr;

    std::map<SOCKET, SOCKADDR_IN> clients;
};

#endif // TCPSERVER_H