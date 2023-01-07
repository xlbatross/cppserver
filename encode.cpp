#include "encode.h"

Encode::Encode()
{

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

    // ���
    // ����� ����(4����Ʈ ������, �� ���̰��� �� �ڿ� ���� �������� ���̸� �ǹ��Ѵ�.)
    //  + ��û Ÿ��(4����Ʈ ������) + ������ �ϳ��� ����Ʈ ����(4����Ʈ ������) * ((����� ���� / 4����Ʈ) - 1)
    headerVector.push_back(type);
    for (int i = 0; i < dataBytesList.size(); i++)
    {
        // ������ �ϳ��� ����Ʈ ���� (4����Ʈ ������)
        headerVector.push_back(dataBytesList[i].size());
        dataSize += dataBytesList[i].size();
    }

    realHeaderSize = headerVector.size() * sizeof(int);
    headerSize = sizeof(int) + realHeaderSize;

    headerBytes = new char[headerSize]();
    memcpy(headerBytes, &realHeaderSize, sizeof(int));
    memcpy(headerBytes + sizeof(int), (char *)headerVector.data(), realHeaderSize);

    // ������
    dataBytes = new char[dataSize];
    for (int i = 0; i < dataBytesList.size(); i++)
    {
        memcpy(dataBytes + dataPointer, dataBytesList[i].data(), dataBytesList[i].size());
        dataPointer += dataBytesList[i].size();
    }
}

EncodeChat::EncodeChat(const std::string &msg)
{
    vector<char> msgvector(msg.length());
    std::copy(msg.begin(), msg.end(), msgvector.begin());
    dataBytesList.push_back(msgvector);

    packaging(Encode::Chat);
}

// ReqImage::ReqImage(const cv::Mat &img)
// {
//     vector<char> imgvector(img.total() * img.channels());
//     std::copy((char *)img.data, (char *)img.data + (img.total() * img.channels()), imgvector.begin());
//     dataBytesList.push_back(imgvector);

//     packaging(Request::Image);
// }
// UDP end
