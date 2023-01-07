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
    LTCPServer();
    ~LTCPServer();

    void start(int port = 2500);
    int acceptClient();
    
    bool receiveData(const int clntSock, Decode * & dcd);
    bool sendData(const int clntSock, Encode * ecd);
    
private:
    std::mutex m;

    int servSock;
    struct sockaddr_in servAddr;

    std::map<int, std::string> clients;

    int receiveBytes(const int clntSock, char * & rawData);
    int sendBytes(const int clntSock, const char *headerBytes, const int headerSize, const char *dataBytes, const int dataSize);
};

#endif // WTCPSERVER_H