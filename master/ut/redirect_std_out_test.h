
#pragma once

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

    wcout.rdbuf( &g_DbgBuf );    // Unicode°æ±¾
    cout.rdbuf( &g_DbgBuf_a );    // ·ÇUnicode°æ±¾

    cout << "hello" << endl;
}