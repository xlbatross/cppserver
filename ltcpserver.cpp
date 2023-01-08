#include "ltcpserver.h"

LTCPServer::LTCPServer()
{
    servSock = socket(PF_INET, SOCK_STREAM, 0);
    
    db = new DB();
}

LTCPServer::~LTCPServer()
{
    delete db;
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
        std::cout << clntSock << " connect" << std::endl;
        clients[clntSock] = "";
        std::cout << "clients : " << clients.size() << std::endl;
        m.unlock();
    }
    return clntSock;
}

int LTCPServer::receiveBytes(const int clntSock, char * & rawData)
{
    char dataSizeBuffer[4];
    // 4바이트를 먼저 읽어, 총 데이터의 길이를 파악한다
    int readBytes = read(clntSock, dataSizeBuffer, 4);
    if (readBytes <= 0)
        return -1;

    // 총 데이터 길이는 헤더의 길이(4바이트) + 헤더 + 실제 데이터
    // 헤더는 요청 응답 타입(4바이트) + 실제 데이터 하나의 길이값(4바이트) * 헤더의 길이 - 1
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

void LTCPServer::receiveData(const int clntSock, Decode * & dcd)
{
    char * rawData = NULL;
    bool isSuccess = false;
    int totalDataSize = receiveBytes(clntSock, rawData);

    if (totalDataSize != -1)
    {
        isSuccess = true;
        dcd = (Decode *)(new DecodeTCP(rawData));
    }
    else
    {
        m.lock();
        std::cout << clntSock << " disconnect" << std::endl;
        if (clients.find(clntSock) != clients.end())
            clients.erase(clntSock);
        std::cout << "clients : " << clients.size() << std::endl;
        close(clntSock);
        m.unlock();
    }
    if (rawData != NULL)
        delete [] rawData;
}


void LTCPServer::sendData(const int clntSock, Encode * ecd)
{
    int totalSendCount = 0;
    switch(ecd->Type())
    {
    case Encode::Chat:
    {
        m.lock();
        for (std::map<int, std::string>::iterator Iter = clients.begin(); Iter != clients.end(); Iter++)
        {
            if(Iter->second != "" && sendBytes(Iter->first, ecd->HeaderBytes(), ecd->HeaderSize(), ecd->DataBytes(), ecd->DataSize()) != -1)
                totalSendCount += 1;
        }
        m.unlock();
    } break;
    case Encode::LoginResult:
    case Encode::RegistResult:
    {
        totalSendCount = sendBytes(clntSock, ecd->HeaderBytes(), ecd->HeaderSize(), ecd->DataBytes(), ecd->DataSize());
    } break;
    }
}

void LTCPServer::processData(const int clntSock, Decode * dcd, Encode * & ecd)
{
    switch(dcd->Type())
    {
        case Decode::Chat:
        {
            std::cout << clntSock << " Chat" << std::endl; 
            DcdChat chat((DecodeTCP *)dcd);
            m.lock();
            if (clients.find(clntSock) != clients.end())
                ecd = (Encode *)(new EcdChat(clients[clntSock], chat.Msg()));
            m.unlock();
        } break;

        case Decode::Login:
        {
            std::cout << clntSock << " Login" << std::endl;
            DcdLogin login((DecodeTCP *)dcd);
            string name = "";
            int loginState = db->login(login.Id(), login.Pw(), name);
            m.lock();
            if (name != "")
            {
                for (std::map<int, std::string>::iterator Iter = clients.begin(); Iter != clients.end(); Iter++)
                {
                    if (name == Iter->second)
                    {
                        loginState = -2;
                        break;
                    }
                }
            }
            
            if (loginState == 1)
            {
                std::cout << clntSock << " loginSuccess" << std::endl;
                clients[clntSock] = name;
                std::cout << clntSock << " name : " << clients[clntSock] << std::endl;
            }
            else
            {
                std::cout << clntSock << " loginFailed " << loginState << std::endl;
            }
            m.unlock();
            ecd = (Encode *)(new EcdLoginResult(loginState));
        } break;

        case Decode::Regist:
        {
            DcdRegist regist((DecodeTCP *)dcd);
            int registState = db->regist(regist.Id(), regist.Pw(), regist.Name());
            ecd = (Encode *)(new EcdRegistResult(registState));
        } break;

        case Decode::Logout:
        {
            std::cout << clntSock << " Logout" << std::endl;
            m.lock();
            if (clients.find(clntSock) != clients.end())
                clients[clntSock] = "";
            m.unlock();
        } break;
    }
}