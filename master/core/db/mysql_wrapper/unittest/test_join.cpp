/*=============================================================================
#
#     FileName: test_join.cpp
#         Desc:
#
#       Author: dantezhu
#        Email: zny2008@gmail.com
#     HomePage: http://www.vimer.cn
#
#      Created: 2011-03-12 20:42:45
#      Version: 0.0.1
#      History:
#               0.0.1 | dantezhu | 2011-03-12 20:42:45 | initialization
#
=============================================================================*/

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include "sql_join.h"
using namespace std;
int main(int argc, char **argv)
{
    SQLJoin sql_join;
    sql_join.add_pair(SQLPair("name","dandan"));
    sql_join.add_pair(SQLPair("sex",1));
    sql_join.add_pair(SQLPair("high",1.1));
    sql_join.add_pair(SQLPair("high",true));

    string v1="v1";

    typedef SQLPair P;
    sql_join
        << P("what",(char*)"go")
        << P("str1","v1")
        << P("iend",false,"like");

    cout << sql_join.keys()
        << endl
        << sql_join.values()
        << endl
        << sql_join.pairs();

    cout << endl;    

    vector<SQLPair>& vecPairs = sql_join.get_vec_pairs();

    for(vector<SQLPair>::iterator it = vecPairs.begin(); it != vecPairs.end(); ++it)
    {
        cout << it->pair() << endl;
    }

    return 0;
}
