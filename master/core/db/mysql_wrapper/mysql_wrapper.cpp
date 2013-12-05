/*=============================================================================
#
#     FileName: mysql_wrapper.cpp
#         Desc: mysql的cpp封装版本
#
#       Author: dantezhu
#        Email: zny2008@gmail.com
#     HomePage: http://www.vimer.cn
#
#      Created: 2011-03-09 19:10:45
#      Version: 0.0.2
#      History:
#               0.0.2 | dantezhu | 2011-07-08 11:27:06 | affectedRows 可以返回0
#               0.0.1 | dantezhu | 2011-03-09 19:10:45 | initialization
#
=============================================================================*/
#include "mysql_wrapper.h"

#ifndef MYSQL_WRAPPER_ERROR
#define MYSQL_WRAPPER_ERROR(fmt, args...) \
    snprintf(m_szErrMsg, sizeof(m_szErrMsg), "[%s][%d][%s]"fmt, \
             __FILE__, __LINE__,__FUNCTION__, ##args)
#endif

#define MYSQL_WRAPPER_COPY(dest,src) \
    if (dest) \
    { \
        delete [] dest; \
        dest = NULL; \
    } \
    if (src) \
    { \
        dest = new char[strlen(src)+1]; \
        snprintf(dest,strlen(src)+1,"%s",src); \
    } \
    else \
    { \
        dest = NULL; \
    }

#define MYSQL_WRAPPER_FREE(ptr) \
    if (ptr) \
    { \
        delete [] ptr; \
        ptr = NULL; \
    }
//=============================================================================
int CMYSQLWrapper::MapName2Index(MYSQL_RES* result, map<string, int>& mapName2Index)
{
    if (!result)
    {
        return -1;
    }
    MYSQL_FIELD *fields = NULL;

    uint32_t num_fields = mysql_num_fields(result);
    fields = mysql_fetch_fields(result);
    if (!fields)
    {
        return -2;
    }
    for(uint32_t i = 0; i < num_fields; i++)
    {
        //printf("Field %u is %s\n", i, fields[i].name);
        mapName2Index[fields[i].name]=i;
    }
    return 0;
}

CMYSQLWrapper::CMYSQLWrapper()
{
    m_szErrMsg[0]='\0';
    m_Database = NULL;

    m_port = MYSQL_WRAPPER_DFT_PORT;

    m_ip = NULL;
    m_user = NULL;
    m_pwd = NULL;
    m_db = NULL;
    m_charset = NULL;

    m_bOpen = false;
}
CMYSQLWrapper::~CMYSQLWrapper()
{
    Close();
    _FreeInitData();
}

const char* CMYSQLWrapper::GetErrMsg()
{
    return m_szErrMsg;
}

void CMYSQLWrapper::_CloseMySQL()
{
    m_bOpen = false;
    if(m_Database)
    {
        mysql_close(m_Database);
        m_Database = NULL;
    }
}
void CMYSQLWrapper::_FreeInitData()
{
    m_port = MYSQL_WRAPPER_DFT_PORT;

    MYSQL_WRAPPER_FREE(m_ip);
    MYSQL_WRAPPER_FREE(m_user);
    MYSQL_WRAPPER_FREE(m_pwd);
    MYSQL_WRAPPER_FREE(m_db);
    MYSQL_WRAPPER_FREE(m_charset);
}

int CMYSQLWrapper::Init(const char* ip, const char* user, const char* pwd, const char* db, uint32_t port, const char* charset)
{
    _FreeInitData();

    m_port = port;

    MYSQL_WRAPPER_COPY(m_ip,ip);
    MYSQL_WRAPPER_COPY(m_user,user);
    MYSQL_WRAPPER_COPY(m_pwd,pwd);
    MYSQL_WRAPPER_COPY(m_db,db);
    MYSQL_WRAPPER_COPY(m_charset,charset);

    return 0;
}

int CMYSQLWrapper::Open()
{
    _CloseMySQL();

    // Initialize MySQL...
    m_Database = mysql_init(NULL);

    // Failed...
    if(!m_Database)
    {
        MYSQL_WRAPPER_ERROR("Error: Unable to initialize MySQL API\n");

        return EMYSQLErrDBInit;
    }

    if (m_charset)
    {
        if (mysql_options(m_Database, MYSQL_SET_CHARSET_NAME, m_charset) != 0) {
            MYSQL_WRAPPER_ERROR("mysql_options MYSQL_SET_CHARSET_NAME Error %u: %s\n",mysql_errno(m_Database), mysql_error(m_Database));
            return EMYSQLErrDBInit;
        }
    }

    /*
    设置重连的官方解释如下:

    MYSQL_OPT_RECONNECT (argument type: my_bool *)

    Enable or disable automatic reconnection to the server if the connection is found to have been lost. 
    Reconnect has been off by default since MySQL 5.0.3; this option is new in 5.0.13 and provides a way 
    to set reconnection behavior explicitly.

    Note: mysql_real_connect() incorrectly reset the MYSQL_OPT_RECONNECT option to its default value 
    before MySQL 5.0.19. Therefore, prior to that version, if you want reconnect to be enabled for each 
    connection, you must call mysql_options() with the MYSQL_OPT_RECONNECT option after each call to 
    mysql_real_connect(). This is not necessary as of 5.0.19: Call mysql_options() only before 
    mysql_real_connect() as usual.*/

#if MYSQL_VERSION_ID >= 50019

    char reconnect = 1;//自动重连的设置

    //set re-conn to true. could use ping to reconn
    if (mysql_options(m_Database, MYSQL_OPT_RECONNECT, &reconnect) != 0) {
        MYSQL_WRAPPER_ERROR("mysql_options MYSQL_OPT_RECONNECT Error %u: %s\n",mysql_errno(m_Database), mysql_error(m_Database));
        return EMYSQLErrDBInit;
    }

#endif

    // Connect to server and check for error...
    if(!mysql_real_connect(m_Database, m_ip, m_user, m_pwd, m_db, m_port, NULL, 0))
    {
        MYSQL_WRAPPER_ERROR("Error: Unable to connect to server[%s]\n",mysql_error(m_Database));

        return EMYSQLErrDBConn;
    }

#if MYSQL_VERSION_ID >= 50013 && MYSQL_VERSION_ID < 50019

    char reconnect = 1;//自动重连的设置

    //set re-conn to true. could use ping to reconn
    if (mysql_options(m_Database, MYSQL_OPT_RECONNECT, &reconnect) != 0) {
        MYSQL_WRAPPER_ERROR("mysql_options MYSQL_OPT_RECONNECT Error %u: %s\n", mysql_errno(m_Database), mysql_error(m_Database));
        return EMYSQLErrDBInit;
    }

#endif

/*#if MYSQL_VERSION_ID < 50013
#warning "MYSQL_VERSION_ID <= 50013, support MYSQL_OPT_RECONNECT default"
#endif*/

    m_bOpen = true;

    return 0;
}

void CMYSQLWrapper::Close()
{
    _CloseMySQL();
}

int CMYSQLWrapper::Query(const char* strSql)
{
    if (!strSql)
    {
        MYSQL_WRAPPER_ERROR("Error: query error,strSql is null\n");
        return EMYSQLErrSysPtr;
    }

    if(!m_Database)
    {
        MYSQL_WRAPPER_ERROR("Error: query error,m_Database is null\n");
        return EMYSQLErrSysPtr;
    }

    if(mysql_query(m_Database, strSql) != 0)
    {
        MYSQL_WRAPPER_ERROR("Error: Unable to query,errMsg=%s\n",mysql_error(m_Database));
        return EMYSQLErrDBExe;
    }
    return 0;
}

int CMYSQLWrapper::Query(const string& strSql)
{
    return this->Query(strSql.c_str());
}

int CMYSQLWrapper::Query(const char* strSql, vector<map<string, MYSQLValue> > &vecData)
{
    int ret = Query(strSql);
    if (ret)
    {
        return ret;
    }

    MYSQL_RES * result = NULL;
    ret = Result(result);
    if (ret)
    {
        return ret;
    }

    StMYSQLRes freeRes(result);//让自动析构

    uint32_t unRecords = mysql_num_rows(result);

    if (0 == unRecords)
    {
        return 0;
    }

    uint32_t num_fields = mysql_num_fields(result);
    MYSQL_FIELD *fields = NULL;
    fields = mysql_fetch_fields(result);
    if (!fields)
    {
        MYSQL_WRAPPER_ERROR("Error mysql_fetch_fields fail");
        return EMYSQLErrSys;
    }

    unsigned long *lengths = NULL;
    MYSQL_ROW row;
    for(uint32_t unIndex = 0; unIndex < unRecords; unIndex++)
    {
        row=mysql_fetch_row(result);
        lengths = mysql_fetch_lengths(result);
        if (!lengths)
        {
            MYSQL_WRAPPER_ERROR("Error mysql_fetch_lengths fail,index:%u",unIndex);
            return EMYSQLErrSys;
        }

        map<string, MYSQLValue> tmpMapData;
        for(uint32_t i = 0; i < num_fields; i++)
        {
            MYSQLValue &value = tmpMapData[fields[i].name];
            value.SetData((char*)row[i], lengths[i]);
        }
        vecData.push_back(tmpMapData);
    }

    return 0;
}

int CMYSQLWrapper::Query(const string& strSql, vector<map<string, MYSQLValue> > &vecData)
{
    return this->Query(strSql.c_str(),vecData);
}

int CMYSQLWrapper::Query(const char* strSql, int& affectedRows)
{
    int ret = Query(strSql);
    if (ret)
    {
        return ret;
    }

    affectedRows = AffectedRows();

    return 0;
}

int CMYSQLWrapper::Query(const string& strSql, int& affectedRows)
{
    return this->Query(strSql.c_str(),affectedRows);
}

int CMYSQLWrapper::Result(MYSQL_RES*& result)
{
    if(!m_Database)
    {
        MYSQL_WRAPPER_ERROR("Error: query error,m_Database is null\n");
        return EMYSQLErrSysPtr;
    }

    // Retrieve query result from server...
    MYSQL_RES *t_Result = NULL;
    t_Result = mysql_store_result(m_Database);

    // Failed...
    if(!t_Result)
    {
        MYSQL_WRAPPER_ERROR("Error: Unable to retrieve result\n");
        return EMYSQLErrDBRes;
    }
    //对外暴露
    result = t_Result;
    return 0;
}

int CMYSQLWrapper::AffectedRows()
{
    if(!m_Database)
    {
        MYSQL_WRAPPER_ERROR("Error: query error,m_Database is null\n");
        return EMYSQLErrSysPtr;
    }

    int ret = mysql_affected_rows(m_Database);
    //Mod-Begin by dantezhu in 2011-07-08 11:27:43
    if(ret<0)
    {
        MYSQL_WRAPPER_ERROR("Error: AffectedRows rows:[%u]\n",ret);
        return EMYSQLErrSys;
    }
    //Mod-End
    return ret;
}

bool CMYSQLWrapper::IsOpen()
{
    return m_bOpen;
}

string CMYSQLWrapper::EscStr(const char* src, uint32_t len)
{
    if(!m_Database)
    {
        MYSQL_WRAPPER_ERROR("Error: query error,m_Database is null\n");
        return "";
    }

    if (!src)
    {
        MYSQL_WRAPPER_ERROR("src is null");
        return "";
    }
    char *escapeBuff = new char[len*2 + 1];

    mysql_real_escape_string(m_Database,escapeBuff,src,len);
    string strDest(escapeBuff);

    delete [] escapeBuff;
    escapeBuff = NULL;

    return strDest;
}

string CMYSQLWrapper::EscStr(const char* src)
{
    if (!src)
    {
        MYSQL_WRAPPER_ERROR("src is null");
        return "";
    }
    uint32_t len = strlen(src);
    return EscStr(src,len);
}

string CMYSQLWrapper::EscStr(const string& src)
{
    return this->EscStr(src.c_str());
}

MYSQL* CMYSQLWrapper::GetMYSQLPtr()
{
    return m_Database;
}
