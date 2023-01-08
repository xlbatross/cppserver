#include "db.h"

DB::DB()
{
    ip = "127.0.0.1";
    user = "root";
    pw = "iotiotiot";
    schema = "chatdb";
    port = 3306;

    if (mysql_library_init(0, NULL, NULL)) 
    {
        cout << "could not initialize MySQL library" << endl;
    }
}

DB::~DB()
{
    mysql_library_end();
}

bool DB::connect(MYSQL * & conn)
{
    conn = mysql_init(conn);
    if (!conn)
    {
        cout << "init error : " << mysql_error(conn) << endl;
        return false;
    }
    if 
    (
        !mysql_real_connect
        (
		    conn,       /* MYSQL structure to use */
		    ip.c_str(),  /* server hostname or IP address */
		    user.c_str(),  /* mysql user */
		    pw.c_str(),   /* password */
		    schema.c_str(),    /* default database to use, NULL for none */
		    port,           /* port number, 0 for default */
		    NULL,        /* socket file or named pipe name */
		    CLIENT_FOUND_ROWS /* connection flags */
        )
    )
    {
        cout << "connect error : " << mysql_error(conn) << endl;
        return false;
    }
    mysql_autocommit(conn, 0);
    return true;
}

bool DB::executeQuery(MYSQL * & conn, const string & query)
{
    if (mysql_query(conn, query.c_str()))
    {
        cout << "query error : " << mysql_error(conn) << endl;
        return false;
    }
    return true;
}

bool DB::getResult(MYSQL_RES * & result, MYSQL * conn)
{
    result = mysql_store_result(conn);
    // 쿼리 실행 결과 구분을 여기서 한다.
    // result가 NULL 일 경우는 검색하여 표시할 결과가 없기 때문에 쿼리문이 SELECT문이 아닌 INSERT, UPDATE, DELETE일 수 있다.
    // 하지만 mysql_field_count가 0이 아니라면 검색된 로우가 있다는 얘기로
    // 앞과 합쳐서 생각해보면 SELECT에 실패했다는 얘기가 된다.
    if (!result && mysql_field_count(conn) != 0)
    {
        cout << "result error: " << mysql_error(conn) << endl;
        return false;
    }
    return true;
}

bool DB::mapResult(MYSQL_RES * result, vector<vector<string>> & list)
{
    MYSQL_ROW row;
    unsigned int i;
    unsigned long *lengths;
    unsigned int num_fields = mysql_num_fields(result);

    try 
    {
        while ((row = mysql_fetch_row(result))) 
        {
            vector<string> rowString;
            lengths = mysql_fetch_lengths(result);
            for (i = 0; i < num_fields; i++) 
            {
                rowString.push_back(string(row[i], lengths[i]));
            }
            list.push_back(rowString);
        }
        return true;
    }
    catch(exception & e)
    {
        cout << e.what() << endl;
        return false;
    }
}

bool DB::commit(MYSQL * conn)
{
    if (mysql_commit(conn))
    {
        cout << "commit error: " << mysql_error(conn) << endl;
        mysql_rollback(conn);
        return false;
    }
    return true;
}

int DB::getUsrInfoList(vector<vector<string>> & list)
{
    MYSQL * conn = NULL;
    MYSQL_RES * result = NULL;
    string query = "SELECT * FROM usrinfo";
    int returnValue = 0;

    if (returnValue >= 0 && !connect(conn))
        returnValue = -1;
    
    if (returnValue >= 0 && !executeQuery(conn, query))
        returnValue = -1;
    
    if (returnValue >= 0 && !getResult(result, conn))
        returnValue = -1;

    if (returnValue >= 0)
        returnValue = (mapResult(result, list)) ? list.size() : -1;

    mysql_free_result(result);
    mysql_close(conn);

    return returnValue;
}

int DB::regist(const string & id, const string & pw, const string & name)
{
    MYSQL * conn = NULL;
    MYSQL_RES * result = NULL;
    // INSERT INTO usrinfo(usrid, usrpw, usrname) SELECT '{id}', '{pw}', '{name}' FROM DUAL WHERE NOT EXISTS (SELECT * FROM usrinfo WHERE usrid = '{id}')
    string query = "INSERT INTO usrinfo(usrid, usrpw, usrname) SELECT '" + id + "', '" + pw + "', '" + name + "' FROM DUAL WHERE NOT EXISTS (SELECT * FROM usrinfo WHERE usrid = '" + id + "')";
    int returnValue = 0;
    
    if (returnValue >= 0 && !connect(conn))
        returnValue = -1;

    if (returnValue >= 0 && !executeQuery(conn, query))
        returnValue = -1;
    
    if (returnValue >= 0 && !getResult(result, conn))
        returnValue = -1;

    if (returnValue >= 0)
    {
        if (mysql_affected_rows(conn) == 0)
            returnValue = -2; // id가 존재한다는 뜻
        else if (!commit(conn))
            returnValue = -1;
    }
        
    mysql_free_result(result);
    mysql_close(conn);

    return returnValue;
}

int DB::login(const string & id, const string & pw, string & name)
{
    MYSQL * conn = NULL;
    MYSQL_RES * result = NULL;
    string query = "SELECT * FROM usrinfo WHERE usrid = '" + id + "' and usrpw = '" + pw + "'";
    int returnValue = 0;
    vector<vector<string>> list;
    
    if (returnValue >= 0 && !connect(conn))
        returnValue = -1;

    if (returnValue >= 0 && !executeQuery(conn, query))
        returnValue = -1;
    
    if (returnValue >= 0 && !getResult(result, conn))
        returnValue = -1;

    if (returnValue >= 0)
    {
        if (mapResult(result, list))
        {
            name = (list.size() > 0) ? list[0][2] : "";
            returnValue = list.size();
        }
        else
            returnValue = -1;
    }
        
    mysql_free_result(result);
    mysql_close(conn);

    return returnValue;
}