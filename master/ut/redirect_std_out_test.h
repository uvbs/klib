
#ifndef _klib_redirect_std_out_test_h
#define _klib_redirect_std_out_test_h

#include <rdebug/redirect_std_out.h>

bool CALLBACK MyOutputDebugString( LPCTSTR lpText )
{
    OutputDebugString( lpText );
    return true;
}

bool CALLBACK MyOutputDebugString_A( LPCSTR lpText )
{
    OutputDebugStringA( lpText );
    return true;
}

TEST(debug, redirect_std_out)
{
    CDebugStreamBuf<TCHAR> g_DbgBuf( MyOutputDebugString );
    CDebugStreamBuf<CHAR> g_DbgBuf_a( MyOutputDebugString_A );

#ifdef _UNICODE
    wcout.rdbuf( &g_DbgBuf );    // Unicode°æ±¾
#endif

    cout.rdbuf( &g_DbgBuf_a );    // ·ÇUnicode°æ±¾

    cout << "hello" << endl;
}

#endif
