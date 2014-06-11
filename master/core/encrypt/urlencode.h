#ifndef _klib_urlencode_h
#define _klib_urlencode_h


namespace klib{
namespace encode {

//----------------------------------------------------------------------
// Summary:
//  “只有字母和数字[0-9a-zA-Z]、一些特殊符号“$-_.+!*'(),”
//  [不包括双引号]、以及某些保留字，才可以不经过编码直接用于URL。”
//----------------------------------------------------------------------
class url_encode {

public:
    BOOL encode(const byte* pszSrc, size_t uLen, std::string& strEncoded)
    {
        std::string strOut;
        byte  uCh = 0;
        for( size_t ix = 0; ix < uLen; ++ix )
        {
            unsigned char buf[4];
            uCh = (byte) pszSrc[ix];
            if( isalnum(uCh ))
            {
                buf[0] = pszSrc[ix];
                buf[1] = '\0';
            }
            else
            {
                buf[0] = '%';
                buf[1] = to_hex( (unsigned char) pszSrc[ix] >> 4 );
                buf[2] = to_hex( (unsigned char) pszSrc[ix] % 16);
                buf[3] = '\0';
            }

            strOut += (char *)buf;
        }

        strEncoded = strOut;

        return TRUE;
    }

    BOOL decode(const byte* pszSrc, size_t uLen, std::string& strDecoded)
    {
        std::string strOut;

        byte uchSrc = 0;
        byte uchDst = 0;

        for(size_t ix = 0; ix < uLen; ++ ix)
        {
            uchSrc = pszSrc[ix];
            uchDst = uchSrc;
            if(uchSrc == '%')
            {
                uchDst = (from_hex(pszSrc[ix + 1]) << 4);
                uchDst |= from_hex(pszSrc[ix + 2]);
                ix += 2;
            }

            strOut += (char) uchDst;
        }
        strDecoded = strOut;

        return TRUE;
    }
    
protected:
    inline unsigned char to_hex(const unsigned char &x)
    {
        return x > 9 ? (x - 10 + 'A') : ( x + '0');
    }

    inline unsigned char from_hex(const unsigned char &x)
    {
        return isdigit(x) ? (x - '0') : (x - 'A' + 10);
    }


};


}} // namespace 

#endif
