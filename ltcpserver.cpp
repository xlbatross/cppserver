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
        m.lock();
        clients[clntSock] = "";
        m.unlock();
    }
    return clntSock;
}

int LTCPServer::receiveBytes(const int clntSock, char * & rawData)
{
    char dataSizeBuffer[4];
    // 4����Ʈ�� ���� �о�, �� �������� ���̸� �ľ��Ѵ�
    int readBytes = read(clntSock, dataSizeBuffer, 4);
    if (readBytes <= 0)
        return -1;

    // �� ������ ���̴� ����� ����(4����Ʈ) + ��� + ���� ������
    // ����� ��û ���� Ÿ��(4����Ʈ) + ���� ������ �ϳ��� ���̰�(4����Ʈ) * ����� ���� - 1
    int totalRecvSize = 0;
    int packetSize = 0; 
    int totalDataSize = *((int *)dataSizeBuffer);
    rawData = new char[totalDataSize];
    while (totalRecvSize < totalDataSize)
    {
        packetSize = (totalRecvSize + 1024 < totalDataSize) ? 1024 : totalDataSize - totalRecvSize;
        readBytes = read(clntSock, rawData + totalRecvSize, packetSize);
        if (readBytes <= 0)
            return -1;
        totalRecvSize += readBytes;
    }
    return totalRecvSize;
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


bool LTCPServer::sendData(const int clntSock, Encode * ecd)
{
    int totalSendCount = 0;
    switch(ecd->Type())
    {
    case Encode::Chat:
    {
        m.lock();
        for (std::map<int, std::string>::iterator Iter = clients.begin(); Iter != clients.end(); Iter++)
        {
            if(sendBytes(Iter->first, ecd->HeaderBytes(), ecd->HeaderSize(), ecd->DataBytes(), ecd->DataSize()) != -1)
                totalSendCount += 1;
        }
        m.unlock();
    } break;
    }

    return (totalDataSize != -1);
}