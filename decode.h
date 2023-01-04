#ifndef DECODE_H
#define DECODE_H

#include <vector>
#include <string>

using std::vector;
using std::string;

class Decode
{
public:
    enum Type {Chat};
    Decode();
    const int Type();
    const vector<vector<char>> & DataBytesList();

protected:
    int type;
    vector<vector<char>> dataBytesList;
};

class DecodeTCP : public Decode
{
public:
    DecodeTCP(const char * rawData);
};

class DecodeChat
{
public:
    DecodeChat(DecodeTCP * dcdtcp);
    const string Msg();
private:
    string msg;
};
// class ReqChat
// {
// public:
//     ReqChat();
//     ReqChat(ResponseTCP * restcp);

//     const string Msg();

// private:
//     string msg;
// };
// // tcp end

// // udp start
// class ResponseUDP : public Response
// {
// public:
//     ResponseUDP();
//     ResponseUDP(const char * rawData, const int totalDataSize);

//     const int SeqNum();

// protected:
//     int seqNum = -1;
// };
//udp end

#endif // DECODE_H
