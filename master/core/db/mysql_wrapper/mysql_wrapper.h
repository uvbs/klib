/*=============================================================================
#
#     FileName: mysql_wrapper.h
#         Desc: mysql的cpp封装版本
#
#       Author: dantezhu
#        Email: zny2008@gmail.com
#     HomePage: http://www.vimer.cn
#
#      Created: 2011-03-09 19:10:45
#      Version: 0.0.2
#      History:
#               0.0.2 | dantezhu | 2011-03-15 20:16:05 | 判断字符串的长度,防止
#                                                      | 转化为随机值
#               0.0.1 | dantezhu | 2011-03-09 19:10:45 | initialization
#
=============================================================================*/
#ifndef __MYSQL_WRAPPER_H__
#define __MYSQL_WRAPPER_H__

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include "mysql.h"

using namespace std;

#define MYSQL_WRAPPER_DFT_PORT      3306

//统一错误返回码
typedef enum {
    //成功
    EMYSQLSuc = 0,

    //系统错误
    EMYSQLErrSys = -1000,           //非法操作
    EMYSQLErrSysMem,                //内存分配失败
    EMYSQLErrSysPtr,                //指针错误

    //DB错误
    EMYSQLErrDBInit = -2000,        //DB初始化失败
    EMYSQLErrDBConn,                //DB连接失败
    EMYSQLErrDBExe,                 //DB执行失败
    EMYSQLErrDBRes,                 //DBResult为空
} EMYSQLRetcode;


//用来给result实现自动析构
//1.这里的析构并不会影响mysql的链接
//2.即使mysql关闭，这块buf也必须手动析构
typedef struct _StMYSQLRes
{
public:
    _StMYSQLRes(MYSQL_RES* result)
    {
        m_result = result;
    }
    ~_StMYSQLRes()
    {
        if(NULL != m_result)
        {
            mysql_free_result(m_result);
            m_result = NULL;
        }
    }

private:
    MYSQL_RES *m_result;
}StMYSQLRes;

class MYSQLValue
{
public:
    MYSQLValue() {
        m_bNull = true;
    }
    virtual ~MYSQLValue() {}

    //设置数据
    int SetData(const char* pData, int iLen)
    {
        if (!pData)
        {
            m_bNull = true;
            return 0;
        }

        m_bNull = false;

        m_strData.resize(iLen+1);
        memcpy((char*)m_strData.data(), pData, iLen);
        ((char*)m_strData.data())[iLen] = '\0';

        return 0;
    }

    int size()
    {
        if (m_bNull)
        {
            return -1;
        }
        return m_strData.size() - 1;
    }

    const char* data()
    {
        if (m_bNull)
        {
            return NULL;
        }
        return m_strData.data();
    }

    template <typename T> 
    T as()
    {
        T asVal;
        pri_as(asVal);

        return asVal;
    }

private:
    int pri_as(char *& val)
    {
        val = (char*)data();
        return 0;
    }
    int pri_as(const char *& val)
    {
        val = data();
        return 0;
    }
    int pri_as(char& val)
    {
        int32_t tmp_val;
        int ret = pri_as(tmp_val);
        val = tmp_val;
        return ret;
    }
    int pri_as(unsigned char& val)
    {
        uint32_t tmp_val;
        int ret = pri_as(tmp_val);
        val = tmp_val;
        return ret;
    }
    int pri_as(string& val)
    {
        if (m_bNull)
        {
            val = "";
        }
        val = m_strData.data();
        return 0;
    }
    template <typename T> 
    int pri_as(T& val)
    {
//Add-Begin by dantezhu in 2011-03-15 20:15:19
//要不然如果是空串的话，转成的int是随机值
        if (this->size() <= 0)
        {
            val = 0;
            return 0;
        }
//Add-End
        stringstream ss;
        ss << this->data();
        ss >> val;
        return 0;
    }

private:
    string m_strData;

    bool m_bNull;
};

class CMYSQLWrapper
{
public:
    /**
     * @brief   静态函数，拿到列名对下标的映射
     *
     * @param   result          取到的result结果
     * @param   mapName2Index   映射数据
     *
     * @return  0               succ
     *          else            fail
     */
    static int MapName2Index(MYSQL_RES* result, map<string, int>& mapName2Index);

public:
    CMYSQLWrapper();
    virtual ~CMYSQLWrapper();

    /**
     * @brief   获取错误信息
     *
     * @return  错误信息
     */
    const char* GetErrMsg();

    /**
     * @brief   初始化MYSQL，并不真正进行连接
     *
     * @param   ip          IP
     * @param   user        用户名
     * @param   pwd         密码(没有则传NULL)
     * @param   db          库(没有则传NULL)
     * @param   port        端口
     * @param   charset     编码(NULL,utf8,gb2312)
     *
     * @return  0           succ
     *          else        fail
     */
    int Init(const char* ip, const char* user, const char* pwd, const char* db, uint32_t port=MYSQL_WRAPPER_DFT_PORT, const char* charset=NULL);

    /**
     * @brief   连接MYSQL，已经支持了自动重连模式，即mysql server关闭链接会自动重连
     *
     * @return  0           succ
     *          else        fail
     */
    int Open();

    /**
     * @brief   关闭链接并释放result
     */
    void Close();

    /**
     * @brief   执行SQL语句
     *
     * @param   strSql      执行语句
     * @param   result      执行结果
     *
     * @return  0           succ
     *          else        fail
     */
    int Query(const char* strSql);
    int Query(const string& strSql);

    /**
     * @brief   针对Read(select)相关的的Query，可以支持blob了
     *
     * @param   strSql          sql语句
     * @param   vecData         rows
     *
     * @return  0               succ
     *          else            fail
     */
    int Query(const char* strSql, vector<map<string, MYSQLValue> > &vecData);
    int Query(const string& strSql, vector<map<string, MYSQLValue> > &vecData);

    /**
     * @brief   针对Write(insert,update,delete)相关的Query
     *
     * @param   strSql          sql语句
     * @param   affectedRows    影响的行的个数
     *
     * @return  0               succ
     *          else            fail
     */
    int Query(const char* strSql, int& affectedRows);
    int Query(const string& strSql, int& affectedRows);


    /**
     * @brief   Select时获取数据，记得手工析构，或者用StMYSQLRes
     *
     * @param   result      执行结果
     *
     * @return  0           succ
     *          else        fail
     */
    int Result(MYSQL_RES *&result);

    /**
     * @brief   返回影响行数
     *
     * @return  >0          succ
     *          0           没有更新
     *          <0          fail
     */
    int AffectedRows();

    /**
     * @brief   判断是否已连接
     *
     * @return  true        已连接
     *          false       未连接
     */
    bool IsOpen();

    /**
     * @brief   主要是将blob转成字符串
     *
     * @param   src         blob源
     * @param   len         长度
     *
     * @return  转化后的字符串
     */
    string EscStr(const char* src,uint32_t len);

    /**
     * @brief   将字符串中的某些字符转化(如')
     *
     * @param   src         字符串
     *
     * @return  转化后的字符串
     */
    string EscStr(const char* src);
    string EscStr(const string& src);

    /**
     * @brief   获取原始的mysql指针，一般情况下不要调用
     *
     * @return  
     */
    MYSQL* GetMYSQLPtr();

protected:
    /**
     * @brief   关闭mysql
     */
    void _CloseMySQL();
    /**
     * @brief   释放掉申请的init相关的内存
     */
    void _FreeInitData();

private:
    char m_szErrMsg[1024];

    uint32_t m_port;
    
    char* m_ip;
    char* m_user;
    char* m_pwd;
    char* m_db;
    char* m_charset;

    MYSQL*      m_Database;

    //是否连接
    bool m_bOpen;
};
#endif
