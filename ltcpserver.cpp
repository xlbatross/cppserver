#include "ltcpserver.h"

LTCPServer::LTCPServer()
{
    servSock = socket(PF_INET, SOCK_STREAM, 0);
}

LTCPServer::~LTCPServer()
{
    close(servSock);
}

void LTCPServer::start(int port)
{
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);

    bind(servSock, (struct sockaddr *)&servAddr, sizeof(servAddr));
    listen(servSock, 100);
}

int LTCPServer::acceptClient()
{
    struct sockaddr_in clntAddr;
    socklen_t sz_clntAddr = sizeof(clntAddr);
    int clntSock = accept(servSock, (struct sockaddr *)&clntAddr, &sz_clntAddr);
    if (clntSock != -1)
    {
        clients[clntSock] = clntAddr;
    }
    return clntSock;
}

int LTCPServer::receiveBytes(const int clntSock, char * & rawData)
{
    int totalDataSize = -1;
    char dataSizeBuffer[4];
    // 4바이트를 먼저 읽어, 총 데이터의 길이를 파악한다
    // 총 데이터 길이는 헤더의 길이(4바이트) + 헤더 + 실제 데이터
    // 헤더는 요청 응답 타입(4바이트) + 실제 데이터 하나의 길이값(4바이트) * 헤더의 길이 - 1
    int readBytes = read(clntSock, dataSizeBuffer, 4);
    int dataSize = *((int *)dataSizeBuffer);
    int packetSize = 0;
    if (readBytes != -1)
    {
        rawData = new char[dataSize];
        for (int i = 0; i < dataSize; i += 1024)
        {
            packetSize = (i + 1024 < dataSize) ? 1024 : dataSize - i;
            readBytes = read(clntSock, rawData + i, packetSize);
            if (readBytes == -1)
                return -1;
            totalDataSize += readBytes;
        }
    }
    return totalDataSize;
}

bool LTCPServer::receiveData(const int clntSock, Decode * & dcd)
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

int LTCPServer::sendBytes(const int clntSock, const char *headerBytes, const int headerSize, const char *dataBytes, const int dataSize)
{
    int totalSendSize = -1;
    int totalSize = headerSize + dataSize;
    int sendSize = -1;

    char * totalBytes = new char[sizeof(int) + totalSize];
    memcpy(totalBytes, (char *)&totalSize, sizeof(int));
    memcpy(totalBytes + sizeof(int), headerBytes, headerSize);
    memcpy(totalBytes + sizeof(int) + headerSize, dataBytes, dataSize);

    if ((sendSize = write(clntSock, totalBytes, sizeof(int) + totalSize)) != -1)
        totalSendSize = sendSize;

    delete [] totalBytes;

    return totalSendSize;
}

bool LTCPServer::sendData(const int clntSock, Encode * ecd)
{
    int totalDataSize = sendBytes(clntSock, ecd->HeaderBytes(), ecd->HeaderSize(), ecd->DataBytes(), ecd->DataSize());

    return (totalDataSize != -1);
}