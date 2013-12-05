/*
 * $ WININET HTTP Client
 * ----------------------------------------------------------------------------------------------------------------
 *
 * name :          wininet_httpclient
 *
 * version tag :     0.1.0
 *
 * description :    HTTP Client using WININET
 *
 * author :          Heo Yongsun ( gooshin@opentown.net )
 *
 * This code distributed under BSD LICENSE STYLE.
 */

 /**
Usage(Example): 

#include "wininet_httpclient.h"

#include <iostream.h>

int main(int argc, char* argv[])
{
	wininet_httpclient *pClient=new wininet_httpclient();

    pClient->InitilizePostArguments();
    pClient->AddPostArguments("xx", "yy");

    if(TRUE){
        pClient->AddPostArguments("__TAG_CHARGE", "Y");
    }else{
        pClient->AddPostArguments("__TAG_CHARGE", "N");
    }
    pClient->AddPostArguments("__TAG_CPCODE", "szCPCode");
    pClient->AddPostArguments("upload_file", "E:\\Project\\client\\OXSolution\\Bin\\OX.exe", TRUE);

    if(pClient->Request("http://uc.app.com/1.php?pararm=lsdjf", 
        wininet_httpclient::RequestPostMethodMultiPartsFormData)){        
        LPCTSTR szResult=pClient->QueryHTTPResponse();
		cout << szResult;
    }else{
#ifdef    _DEBUG
        LPVOID     lpMsgBuffer;
        DWORD dwRet=FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER |
                      FORMAT_MESSAGE_FROM_SYSTEM,
                      NULL,
                      pClient->GetLastError(),
                      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                      reinterpret_cast<LPTSTR>(&lpMsgBuffer),
                      0,
                      NULL);
        OutputDebugString(reinterpret_cast<LPTSTR>(lpMsgBuffer));
        LocalFree(lpMsgBuffer);
#endif
    }

	//pClient->

	return 0;
}

////
wininet_httpclient    httpClient;

if(httpRequest.Request("http://www.codeproject.com")){
    LPCTSTR szHTML=httpRequest.QueryResponse();
}else{
    LPVOID     lpMsgBuffer;
    DWORD dwRet=FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | 
             FORMAT_MESSAGE_FROM_SYSTEM,
             NULL,
             httpRequest.GetLastError(),
             MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
             reinterpret_cast<LPTSTR>(&lpMsgBuffer),
             0,
             NULL);                         

    MessageBox( reinterpret_cast<LPTSTR>(lpMsgBuffer), "ERROR", MB_OK);
    LocalFree(lpMsgBuffer);
}

//////////
º¯ÊýËµÃ÷:
Connect(...) method connects to HTTP Server.
Close() method closes connection. These are used with RequestOfURI(...).
InitilizePostArguments() method initializes post arguments.
AddPostArguments(...) method is supported so that you can add new post arguments of the following 3 types ( TCHAR, DWORD, FILE).
Request(...) method is for you to attempt request for HTTP REQUEST( GET, POST, POST-MULTIPARTFORMDATA) with URL. HTTP METHOD indirector have 3 types.
wininet_httpclient::GetMethod is HTTP GET REQUEST
wininet_httpclient::PostMethod is HTTP POST REQUEST
wininet_httpclient::PostMultiPartsFormData is HTTP POST REQUEST with BINARY FORM DATA
RequestOfURI(...) method is that you could have attempt request for HTTP REQUEST with URI. This method is used with Connect(...) and Close().
Response(...) method is that you have HTTP Response by BYTES.
QueryHTTPResponse() method is you have receive HTML of your HTTP REQUEST.
QueryHTTPResponseHeader() method is you have receive HTTP HEADER about QueryHTTPResponse().
GetLastError() method is you get windows error code.
GetContentType(...) method is you have get MIME-TYPE.
ParseURL(...) method parse URL to protocol(HTTP, HTTPS) and address(or hostname) and port, URI.

 */

#ifndef __GENERIC_HTTP_CLIENT
#define __GENERIC_HTTP_CLIENT

//#include <afxwin.h>

#include <WinSock2.h>
#include <windows.h>


#include <winreg.h>
#include <tchar.h>
#include <wininet.h>

// use stl
#include <vector>

namespace klib {
namespace net {


// PRE-DEFINED CONSTANTS
#define __DEFAULT_AGENT_NAME	_T("MERONG(0.9/;p)")

// PRE-DEFINED BUFFER SIZE
#define	__SIZE_HTTP_ARGUMENT_NAME	256
#define __SIZE_HTTP_ARGUMENT_VALUE	1024

#define __HTTP_VERB_GET	    _T("GET")
#define __HTTP_VERB_POST    _T("POST")
#define __HTTP_ACCEPT_TYPE  _T("*/*")
#define __HTTP_ACCEPT       _T("Accept: */*\r\n")
#define __SIZE_HTTP_BUFFER	100000
#define __SIZE_HTTP_RESPONSE_BUFFER	10*1024
#define __SIZE_HTTP_HEAD_LINE	2048

#define __SIZE_BUFFER	1024
#define __SIZE_SMALL_BUFFER	256

class wininet_httpclient {
public:					
	typedef struct __GENERIC_HTTP_ARGUMENT{							// ARGUMENTS STRUCTURE
		TCHAR	szName[__SIZE_HTTP_ARGUMENT_NAME];
		TCHAR	szValue[__SIZE_HTTP_ARGUMENT_VALUE];
		DWORD	dwType;
		bool operator==(const __GENERIC_HTTP_ARGUMENT &argV){
			return !_tcscmp(szName, argV.szName) && !_tcscmp(szValue, argV.szValue);
		}
	} GenericHTTPArgument;

	enum RequestMethod{															// REQUEST METHOD
		RequestUnknown=0,
		RequestGetMethod=1,
		RequestPostMethod=2,
		RequestPostMethodMultiPartsFormData=3
	};

	enum TypePostArgument{													// POST TYPE 
		TypeUnknown=0,
		TypeNormal=1,
		TypeBinary=2
	};

	// CONSTRUCTOR & DESTRUCTOR
	wininet_httpclient();
	virtual ~wininet_httpclient();

	static wininet_httpclient::RequestMethod GetMethod(int nMethod);
	static wininet_httpclient::TypePostArgument GetPostArgumentType(int nType);

	// Connection handler	
	BOOL Connect(LPCTSTR szAddress, LPCTSTR szAgent = __DEFAULT_AGENT_NAME, unsigned short nPort = INTERNET_DEFAULT_HTTP_PORT, LPCTSTR szUserAccount = NULL, LPCTSTR szPassword = NULL);
	BOOL Close();
	VOID InitilizePostArguments();

	// HTTP Arguments handler	
	VOID AddPostArguments(LPCTSTR szName, DWORD nValue);
	VOID AddPostArguments(LPCTSTR szName, LPCTSTR szValue, BOOL bBinary = FALSE);

	// HTTP Method handler 
	BOOL Request(LPCTSTR szURL, int nMethod = wininet_httpclient::RequestGetMethod, LPCTSTR szAgent = __DEFAULT_AGENT_NAME);
	BOOL RequestOfURI(LPCTSTR szURI, int nMethod = wininet_httpclient::RequestGetMethod);
	BOOL Response(PBYTE pHeaderBuffer, DWORD dwHeaderBufferLength, PBYTE pBuffer, DWORD dwBufferLength, DWORD &dwResultSize);	
	LPCTSTR QueryHTTPResponse();
	LPCTSTR QueryHTTPResponseHeader();	

	// General Handler
	DWORD GetLastError();
	LPCTSTR GetContentType(LPCTSTR szName);
	VOID ParseURL(LPCTSTR szURL, LPTSTR szProtocol, LPTSTR szAddress, DWORD &dwPort, LPTSTR szURI);

	
protected:				
	std::vector<GenericHTTPArgument> _vArguments;				// POST ARGUMENTS VECTOR

	TCHAR		_szHTTPResponseHTML[__SIZE_HTTP_BUFFER];		// RECEIVE HTTP BODY
	TCHAR		_szHTTPResponseHeader[__SIZE_HTTP_BUFFER];	// RECEIVE HTTP HEADR

	HINTERNET _hHTTPOpen;				// internet open handle
	HINTERNET _hHTTPConnection;		// internet connection hadle
	HINTERNET _hHTTPRequest;		// internet request hadle

	DWORD		_dwError;					// LAST ERROR CODE
	LPCTSTR		_szHost;					 //	 HOST NAME
	DWORD		_dwPort;					//  PORT

	// HTTP Method handler
	DWORD ResponseOfBytes(PBYTE pBuffer, DWORD dwSize);
	DWORD GetPostArguments(LPTSTR szArguments, DWORD dwLength);
	BOOL RequestPost(LPCTSTR szURI);
	BOOL RequestPostMultiPartsFormData(LPCTSTR szURI);
	BOOL RequestGet(LPCTSTR szURI);
	DWORD AllocMultiPartsFormData(PBYTE &pInBuffer, LPCTSTR szBoundary = _T("--MULTI-PARTS-FORM-DATA-BOUNDARY-"));
	VOID FreeMultiPartsFormData(PBYTE &pBuffer);
	DWORD GetMultiPartsFormDataLength();
};


}}



#endif	// #ifndef __GENERIC_HTTP_CLIENT
