#ifndef ENCODE_H
#define ENCODE_H

#include <vector>
#include <string>

using std::vector;
using std::string;

class Encode
{
public:
    enum EncodeType {Chat, LoginResult, RegistResult};
    Encode();
    ~Encode();

    virtual void packaging(const int type) = 0;

    const int Type();

    const int HeaderSize();
    const char * HeaderBytes();

    const int DataSize();
    const char * DataBytes();

protected:
    int type = -1;
    vector<vector<char>> dataBytesList;

    int headerSize = 0;
    char * headerBytes = NULL;
    
    int dataSize = 0;
    char * dataBytes = NULL;
};

class EncodeTCP : public Encode
{
public:
    EncodeTCP();
    void packaging(const int type) override;
};

class EcdChat: public EncodeTCP
{
public:
    EcdChat(const string & name, const string & msg);
};

class EcdLoginResult : public EncodeTCP
{
public:
    EcdLoginResult(const int loginState);
};

class EcdRegistResult : public EncodeTCP
{
public:
    EcdRegistResult(const int registState);
};


#endif // ENCODE_H
