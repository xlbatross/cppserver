#include "tcpserver.h"

TCPServer::TCPServer()
{
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    servSock = socket(PF_INET, SOCK_STREAM, 0);
}

TCPServer::~TCPServer()
{
    closesocket(servSock);
    WSACleanup();
}

void TCPServer::start(int port)
{
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);

    bind(servSock, (SOCKADDR *)&servAddr, sizeof(servAddr));
    listen(servSock, 100);
}

SOCKET TCPServer::acceptClient()
{
    SOCKADDR_IN clntAddr;
    int sz_clntAddr = sizeof(clntAddr);
    SOCKET clntSock = accept(servSock, (SOCKADDR *)&clntAddr, &sz_clntAddr);
    if (clntSock != INVALID_SOCKET)
    {
        clients[clntSock] = clntAddr;
    }
    return clntSock;
}

int TCPServer::receiveBytes(const SOCKET clntSock, char * & rawData)
{
    int totalDataSize = -1;
    char dataSizeBuffer[4];
    // 4바이트를 먼저 읽어, 총 데이터의 길이를 파악한다
    // 총 데이터 길이는 헤더의 길이(4바이트) + 헤더 + 실제 데이터
    // 헤더는 요청 응답 타입(4바이트) + 실제 데이터 하나의 길이값(4바이트) * 헤더의 길이 - 1
    int readBytes = recv(clntSock, dataSizeBuffer, 4, 0);
    int dataSize = *((int *)dataSizeBuffer);
    int packetSize = 0;
    if (readBytes != SOCKET_ERROR)
    {
        rawData = new char[dataSize];
        for (int i = 0; i < dataSize; i += 1024)
        {
            packetSize = (i + 1024 < dataSize) ? 1024 : dataSize - i;
            readBytes = recv(clntSock, rawData + i, packetSize, 0);
            if (readBytes == SOCKET_ERROR)
                return -1;
            totalDataSize += readBytes;
        }
    }
    return totalDataSize;
}

bool TCPServer::receiveData(const SOCKET clntSock, Decode * & dcd)
{
    char * rawData = NULL;
    bool isSuccess = false;
    int totalDataSize = receiveBytes(clntSock, rawData);

    if (totalDataSize != -1)
    {
        isSuccess = true;
        dcd = (Decode *)(new DecodeTCP(rawData));
    }
    if (rawData != NULL)
        delete [] rawData;
    return isSuccess;
}

int TCPServer::sendBytes(const SOCKET clntSock, const char *headerBytes, const int headerSize, const char *dataBytes, const int dataSize)
{
    int totalSendSize = -1;
    int totalSize = headerSize + dataSize;
    int sendSize = -1;

    char * totalBytes = new char[sizeof(int) + totalSize];
    memcpy(totalBytes, (char *)&totalSize, sizeof(int));
    memcpy(totalBytes + sizeof(int), headerBytes, headerSize);
    memcpy(totalBytes + sizeof(int) + headerSize, dataBytes, dataSize);

    if ((sendSize = send(clntSock, totalBytes, sizeof(int) + totalSize, 0) != SOCKET_ERROR))
        totalSendSize = sendSize;

    delete [] totalBytes;

//    if ((sendSize = send(sock, (char *)&totalSize, sizeof(int), 0)) == SOCKET_ERROR)
//        return -1;
//    totalSendSize = sendSize;

//    if ((sendSize = send(sock, headerBytes, headerSize, 0)) == SOCKET_ERROR)
//        return -1;
//    totalSendSize += sendSize;

//    if ((sendSize = send(sock, dataBytes, dataSize, 0)) == SOCKET_ERROR)
//        return -1;
//    totalSendSize += sendSize;

    return totalSendSize;
}

bool TCPServer::sendData(const SOCKET clntSock, Encode * ecd)
{
    int totalDataSize = sendBytes(clntSock, ecd->HeaderBytes(), ecd->HeaderSize(), ecd->DataBytes(), ecd->DataSize());

    return (totalDataSize != -1);
}