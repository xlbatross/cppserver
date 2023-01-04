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

    // ó�� 4����Ʈ�� ����� ����
    memcpy(&headerSize, rawData + pointer, sizeof(int));
    pointer += sizeof(int);

    // ���� 4����Ʈ�� ��û �Ǵ� ���� Ÿ��
    memcpy(&type, rawData + pointer, sizeof(int));
    pointer += sizeof(int);

    // ���� 4����Ʈ���� ��� �������� �ǹ��ִ� ������ �ϳ��� ���̰���
    for (int i = 0; i < headerSize - sizeof(int); i += sizeof(int))
    {
        memcpy(&dataLength, rawData + pointer, sizeof(int));
        dataLengthList.push_back(dataLength);
        pointer += sizeof(int);
    }

    if (dataLengthList.size() > 0)
    {
        // ���� �ο쵥���͸� �ǹ��ִ� �����͵�� ��ȯ
        for (int i = 0; i < dataLengthList.size(); i++)
        {
            vector<char> dataBytes(dataLengthList[i]);
            std::copy(rawData + pointer, rawData + pointer + dataLengthList[i], dataBytes.begin());
            dataBytesList.push_back(dataBytes);
            pointer += dataLengthList[i];
        }
    }
}

DecodeChat::DecodeChat(DecodeTCP *dcdtcp)
{
    msg.append(dcdtcp->DataBytesList()[0].data(), dcdtcp->DataBytesList()[0].size());
}

const string DecodeChat::Msg()
{
    return msg;
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



