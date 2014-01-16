// kl_ex.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#pragma warning(disable: 4996)

#include "../kl/kl.h"
#include "../kl/variable.h"
#include "../kl/vmachine.h"


#include <io/mem_seg_stream.h>

#include <stdio.h>

std::string get_content(std::string file)
{
    FILE* hfile = fopen(file.c_str(), "rb");
    if (NULL == hfile) {
        return std::string();
    }

    std::string content;

    char szbuff[1024];
    while (!feof(hfile))
    {
        size_t len = fread(szbuff, 1, sizeof(szbuff), hfile);
        if (len > 0) {
            content.append(std::string(szbuff, len));
        }
    }
    fclose(hfile);
    return std::move(content);
}

void test_seg_buff()
{
    typedef klib::io::mem_seg_stream<11, char> test_buff_type;

    test_buff_type s;
    s.write("abcdefghijklmnopqrst1234567890QWERTYUIOP", 30);

    test_buff_type::value_type buff[2048];
    s.read(buff, s.size());
    // s.read();
}

int _tmain(int argc, _TCHAR* argv[])
{
    test_seg_buff();

    variable var;
    var = 10;
    var = "hello";
    var = 1.1;
    variable str_val = "today is a good day";

    var["hello"] = "world";
    var["key"]   = str_val;
    var["key"]["key2"]["key3"] = "value";
    variable x = std::move(var);


    std::string content = get_content("test.script");

    vmachine vm;
    vm.compile(content.c_str());

	return 0;
}

