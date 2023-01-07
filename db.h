#ifndef DB_H
#define DB_H

#include <iostream>
#include <string>
#include <vector>
#include <mysql.h>
#include <exception>

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::exception;

class DB
{
public:
    DB();
    ~DB();

    int getUsrInfoList(vector<vector<string>> & list);
    int regist(const string & id, const string & pw, const string & name);
    int login(const string & id, const string & pw, string & name);

private:
    string ip;
    string user;
    string pw;
    string schema;
    int port;

    bool connect(MYSQL * & conn);
    bool executeQuery(MYSQL * & conn, const string & query);
    bool getResult(MYSQL_RES * & result, MYSQL * conn);
    bool mapResult(MYSQL_RES * result, vector<vector<string>> & list);
    bool commit(MYSQL * conn);
};

#endif // DB_H