// HttpDwonload.cpp: implementation of the http_down class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include "http_down.h"

#include "addr_resolver.h"
#include "http/http.h"
using namespace klib::net;

#pragma warning(disable: 4996)


bool http_down::download(const char* http_url, const char* pszSaveFile)
{
    http http_;
    return http_.download(http_url, pszSaveFile);
}

bool http_down::get_url_content(const char* http_url, std::string& str_content)
{
    http http_;
    return http_.get_content(http_url, str_content);
}
