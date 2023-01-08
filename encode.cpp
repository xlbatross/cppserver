#include "encode.h"

Encode::Encode()
{
    type = -1;

    headerSize = 0;
    headerBytes = NULL;
    
    dataSize = 0;
    dataBytes = NULL;
}

Encode::~Encode()
{
    if (headerBytes != NULL)
        delete [] headerBytes;
    if (dataBytes != NULL)
        delete [] dataBytes;
}

const int Encode::Type()
{
    return type;
}

const int Encode::HeaderSize()
{
    return headerSize;
}

const char *Encode::HeaderBytes()
{
    return headerBytes;
}

const int Encode::DataSize()
{
    return dataSize;
}

const char *Encode::DataBytes()
{
    return dataBytes;
}

// TCP start
EncodeTCP::EncodeTCP()
{

}

void EncodeTCP::packaging(const int type)
{
    vector<int> headerVector;
    int realHeaderSize = 0;
    int dataPointer = 0;
    this->type = type;

    // 헤더
    // 헤더의 길이(4바이트 정수형, 이 길이값은 이 뒤에 오는 데이터의 길이를 의미한다.)
    //  + 요청 타입(4바이트 정수형) + 데이터 하나의 바이트 길이(4바이트 정수형) * ((헤더의 길이 / 4바이트) - 1)
    headerVector.push_back(type);
    for (int i = 0; i < dataBytesList.size(); i++)
    {
        // 데이터 하나의 바이트 길이 (4바이트 정수형)
        headerVector.push_back(dataBytesList[i].size());
        dataSize += dataBytesList[i].size();
    }

    realHeaderSize = headerVector.size() * sizeof(int);
    headerSize = sizeof(int) + realHeaderSize;

    headerBytes = new char[headerSize]();
    memcpy(headerBytes, &realHeaderSize, sizeof(int));
    memcpy(headerBytes + sizeof(int), (char *)headerVector.data(), realHeaderSize);

    // 데이터
    dataBytes = new char[dataSize];
    for (int i = 0; i < dataBytesList.size(); i++)
    {
        memcpy(dataBytes + dataPointer, dataBytesList[i].data(), dataBytesList[i].size());
        dataPointer += dataBytesList[i].size();
    }
}

EcdChat::EcdChat(const std::string & name, const std::string &msg)
{
    vector<char> namevector(name.length());
    std::copy(name.begin(), name.end(), namevector.begin());
    dataBytesList.push_back(namevector);

    vector<char> msgvector(msg.length());
    std::copy(msg.begin(), msg.end(), msgvector.begin());
    dataBytesList.push_back(msgvector);

    packaging(Encode::Chat);
}

EcdLoginResult::EcdLoginResult(const int loginState)
{
    vector<char> statevector(4);
    std::copy((char *)&loginState, (char *)&loginState + 4, statevector.begin());
    dataBytesList.push_back(statevector);

    packaging(Encode::LoginResult);
}

EcdRegistResult::EcdRegistResult(const int registState)
{
    vector<char> statevector(4);
    std::copy((char *)&registState, (char *)&registState + 4, statevector.begin());
    dataBytesList.push_back(statevector);

    packaging(Encode::RegistResult);
}

// ReqImage::ReqImage(const cv::Mat &img)
// {
//     vector<char> imgvector(img.total() * img.channels());
//     std::copy((char *)img.data, (char *)img.data + (img.total() * img.channels()), imgvector.begin());
//     dataBytesList.push_back(imgvector);

//     packaging(Request::Image);
// }
// UDP end
