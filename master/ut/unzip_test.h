
#pragma once

#include <comp/unzip/ziphelper.h>
#pragma comment(lib, "unzip")
#pragma comment(lib, "zlib")


TEST(unzip, 1)
{
    
    ziphelper helpe;
    helpe.open("C:\\Users\\Administrator\\Downloads\\cef_binary_1.1180.832_windows.zip");
    helpe.extract("d:\\ziptest\\");

}