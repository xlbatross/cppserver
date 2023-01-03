#ifndef LTCPSERVER_H
#define LTCPSERVER_H

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <map>
#include <mutex>
#include <iostream>
#include "decode.h"
#include "encode.h"

class LTCPServer
{
public:
    std::mutex m;

    LTCPServer();
    ~LTCPServer();

    void start(int port = 2500);
    int acceptClient();

    int receiveBytes(const int clntSock, char * & rawData);
    bool receiveData(const int clntSock, Decode * & dcd);

    int sendBytes(const int clntSock, const char *headerBytes, const int headerSize, const char *dataBytes, const int dataSize);
    bool sendData(const int clntSock, Encode * ecd);
    
private:
    int servSock;
    struct sockaddr_in servAddr;

    std::map<int, struct sockaddr_in> clients;
};

#endif // WTCPSERVER_H