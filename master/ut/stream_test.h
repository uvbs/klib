
#pragma once

#include <io/stream.h>

using namespace klib::io;

TEST(stream,1 )
{

    memory_stream stream_;
    stream_.write("sdfdsf", 5);
    
    file_stream fs;
    fs.open("d:\\test.txt", FM_CREATE);
    fs.write("hello\n", 6);



}