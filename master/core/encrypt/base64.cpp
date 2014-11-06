//#include "stdafx.h"
#include "base64.h"
using namespace klib::encode;

#include "../core/scope_guard.h"
using namespace klib::core;


#define BASE64_CODE_LENGTH  65
#define BASE64_NULLCHAR     0x40

char g_Base64Table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";


base64::base64()
{
}

void base64::encode(const std::string& str_in, std::string& str_out, bool add_crlf)
{
	encode(str_in.c_str(), str_in.size(), str_out, add_crlf);
}

void base64::encode(const char* input,size_t l,std::string& output, bool add_crlf)
{
    return encode((const unsigned char*) input, l, output, add_crlf);
}

void base64::encode(const unsigned char* input, 
    size_t nLength,
    std::string& output,
    bool add_crlf)
{
    // Base64的编码需要的空间长度是源串长度的4/3 + 64；
    int nDestLength =  (int)(nLength * 1.7) + 64;
    char *out_buf_ptr = new char[nDestLength];
    if(out_buf_ptr == NULL)
        return ;
    
    defer( delete[] out_buf_ptr  );

    int i = 0;
    int j = 0;
    char *cur_ptr = (char*) input;
    for (i=0, j=0; i<nLength ; i+=3, j+=4)
    {
        if (add_crlf && j % 76 == 0) {
            out_buf_ptr[j]   = '\r';
            out_buf_ptr[j+1] = '\n';
            j += 2;
        }

        out_buf_ptr[j] = g_Base64Table[ (cur_ptr[i] >> 2) & 0x3F ];
        switch(nLength - i)
        {
        case 1:
            out_buf_ptr[j+1] = g_Base64Table[ (cur_ptr[i] << 4) & 0x30 ];
            out_buf_ptr[j+2] = g_Base64Table[ BASE64_NULLCHAR ];
            out_buf_ptr[j+3] = g_Base64Table[ BASE64_NULLCHAR ];
            break;
        case 2:
            out_buf_ptr[j+1] = g_Base64Table[ ((cur_ptr[i] << 4) & 0x30) | ((cur_ptr[i+1] >> 4) & 0x0F) ];
            out_buf_ptr[j+2] = g_Base64Table[ (cur_ptr[i+1] << 2) & 0x3C ];
            out_buf_ptr[j+3] = g_Base64Table[ BASE64_NULLCHAR ];
            break;
        default: // >=3
            out_buf_ptr[j+1] = g_Base64Table[ ((cur_ptr[i] << 4) & 0x30) | ((cur_ptr[i+1] >> 4) & 0x0F) ];
            out_buf_ptr[j+2] = g_Base64Table[ ((cur_ptr[i+1] << 2) & 0x3C) | ((cur_ptr[i+2] >> 6) & 0x03) ];
            out_buf_ptr[j+3] = g_Base64Table[ cur_ptr[i+2] & 0x3F ];
            break;
        }
    }
    out_buf_ptr[j]=0;
    output.assign(out_buf_ptr);
}


void base64::decode(const std::string& input, std::string& output)
{
    // 合法的Base64编码后字符串长度必须为4字节的倍数(不含'\0')
    if(input.size() % 4 !=0 || input.size() <= 0)
    {
        return ;
    }
    output.clear();

    int index;
    unsigned char ch1, ch2, ch3, ch4;
    unsigned char decode_tbl[256];
    memset(decode_tbl,0,256);
    int decode_length = 0;

    for (index = 0; index < BASE64_CODE_LENGTH; index++)
    {
        decode_tbl[(int) g_Base64Table[index]]=(unsigned char) index;
    }

    // uint8_t* szDestBuf = (uint8_t*) pDestBuf;
    for (index = 0; index < input.size(); index += 4)
    {
        char ch = input[index];
        if (ch == '\r' || ch == '\n') {
            index ++;
            continue;
        }

        ch1 = decode_tbl[(int) input[index]];
        ch2 = decode_tbl[(int) input[index + 1]];
        ch3 = decode_tbl[(int) input[index + 2]];
        ch4 = decode_tbl[(int) input[index + 3]];

        output.append(1, ((ch1 << 2) & 0xFC) | ((ch2 >> 4) & 0x03));
        decode_length ++;

        if (ch3 != BASE64_NULLCHAR)
        {
            output.append(1, ((ch2 << 4) & 0xF0) | ((ch3 >> 2) & 0x0F));
            decode_length ++;
            if (ch4 != BASE64_NULLCHAR)
            {
                output.append(1, ((ch3 << 6) & 0xC0) | (ch4 & 0x3F));
                decode_length ++;
            }
        }
    }
}

size_t base64::decode_length(const std::string& str64)
{
    if (!str64.size() || str64.size() % 4)
        return 0;
    size_t l = 3 * (str64.size() / 4 - 1) + 1;
    if (str64[str64.size() - 2] != '=')
        l++;
    if (str64[str64.size() - 1] != '=')
        l++;
    return l;
}


