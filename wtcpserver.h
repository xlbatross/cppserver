#ifndef WTCPSERVER_H
#define WTCPSERVER_H

#include <winsock2.h>
#include <map>
#include <mutex>
#include <iostream>
#include "decode.h"
#include "encode.h"
#include "db.h"

class WTCPServer
{
public:
    WTCPServer();
    ~WTCPServer();

    void start(int port = 2500);
    SOCKET acceptClient();

    bool receiveData(const SOCKET clntSock, Decode * & dcd);
    bool sendData(const SOCKET clntSock, Encode * ecd);
    bool processData(const SOCKET clntSock, Decode * dcd, Encode * & ecd);

private:
    std::mutex m;

    DB * db;

    WSADATA wsaData;

    SOCKET servSock;
    SOCKADDR_IN servAddr;

    std::map<SOCKET, std::string> clients;

    int receiveBytes(const SOCKET clntSock, char * & rawData);
    int sendBytes(const SOCKET clntSock, const char *headerBytes, const int headerSize, const char *dataBytes, const int dataSize);
};

#endif // WTCPSERVER_H