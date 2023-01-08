#include "decode.h"

Decode::Decode()
{

}

const int Decode::Type()
{
    return type;
}

const vector<vector<char> > &Decode::DataBytesList()
{
    return dataBytesList;
}

// tcp start
DecodeTCP::DecodeTCP(const char * rawData)
{
    int pointer = 0;
    int headerSize = 0;
    int dataLength = 0;
    vector<int> dataLengthList;

    // 처음 4바이트는 헤더의 길이
    memcpy(&headerSize, rawData + pointer, sizeof(int));
    pointer += sizeof(int);

    // 다음 4바이트는 요청 또는 응답 타입
    memcpy(&type, rawData + pointer, sizeof(int));
    pointer += sizeof(int);

    // 다음 4바이트부터 헤더 끝까지는 의미있는 데이터 하나의 길이값들
    for (int i = 0; i < headerSize - sizeof(int); i += sizeof(int))
    {
        memcpy(&dataLength, rawData + pointer, sizeof(int));
        dataLengthList.push_back(dataLength);
        pointer += sizeof(int);
    }

    if (dataLengthList.size() > 0)
    {
        // 남은 로우데이터를 의미있는 데이터들로 변환
        for (int i = 0; i < dataLengthList.size(); i++)
        {
            vector<char> dataBytes(dataLengthList[i]);
            std::copy(rawData + pointer, rawData + pointer + dataLengthList[i], dataBytes.begin());
            dataBytesList.push_back(dataBytes);
            pointer += dataLengthList[i];
        }
    }
}

DcdChat::DcdChat(DecodeTCP *dcdtcp)
{
    msg.append(dcdtcp->DataBytesList()[0].data(), dcdtcp->DataBytesList()[0].size());
}

const string DcdChat::Msg()
{
    return msg;
}

DcdLogin::DcdLogin(DecodeTCP *dcdtcp)
{
    id.append(dcdtcp->DataBytesList()[0].data(), dcdtcp->DataBytesList()[0].size());
    pw.append(dcdtcp->DataBytesList()[1].data(), dcdtcp->DataBytesList()[1].size());
}

const string DcdLogin::Id()
{
    return id;
}

const string DcdLogin::Pw()
{
    return pw;
}

DcdRegist::DcdRegist(DecodeTCP *dcdtcp)
{
    id.append(dcdtcp->DataBytesList()[0].data(), dcdtcp->DataBytesList()[0].size());
    pw.append(dcdtcp->DataBytesList()[1].data(), dcdtcp->DataBytesList()[1].size());
    name.append(dcdtcp->DataBytesList()[2].data(), dcdtcp->DataBytesList()[2].size());
}

const string DcdRegist::Id()
{
    return id;
}

const string DcdRegist::Pw()
{
    return pw;
}

const string DcdRegist::Name()
{
    return name;
}
// // udp start
// ResponseUDP::ResponseUDP()
// {

// }

// ResponseUDP::ResponseUDP(const char *rawData, const int totalDataSize)
// {
//     int pointer = 0;

//     memcpy(&responseType, rawData + pointer, sizeof(int));
//     pointer += sizeof(int);

//     memcpy(&seqNum, rawData + pointer, sizeof(int));
//     pointer += sizeof(int);

//     vector<char> dataBytes(totalDataSize - pointer);
//     std::copy(rawData + pointer, rawData + totalDataSize, dataBytes.begin());
//     dataBytesList.push_back(dataBytes);
// }

// const int ResponseUDP::SeqNum()
// {
//     return seqNum;
// }
// // udp end



