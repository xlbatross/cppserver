#ifndef ENCODE_H
#define ENCODE_H

#include <vector>
#include <string>

using std::vector;
using std::string;

class Encode
{
public:
    enum EncodeType {Chat};
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

class EncodeChat: public EncodeTCP
{
public:
    EncodeChat(const string & msg);
};

#endif // ENCODE_H
