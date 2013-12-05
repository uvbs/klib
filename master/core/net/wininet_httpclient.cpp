// wininet_httpclient.cpp: implementation of the wininet_httpclient class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include "wininet_httpclient.h"


using namespace klib::net;

#pragma comment(lib, "wininet.lib")

wininet_httpclient::wininet_httpclient(){
	_hHTTPOpen=_hHTTPConnection=_hHTTPRequest=NULL;
}

wininet_httpclient::~wininet_httpclient(){
	_hHTTPOpen=_hHTTPConnection=_hHTTPRequest=NULL;
}

wininet_httpclient::RequestMethod wininet_httpclient::GetMethod(int nMethod){	
	return nMethod<=0 ? wininet_httpclient::RequestGetMethod : static_cast<wininet_httpclient::RequestMethod>(nMethod);
}

wininet_httpclient::TypePostArgument wininet_httpclient::GetPostArgumentType(int nType){
	return nType<=0 ? wininet_httpclient::TypeNormal : static_cast<wininet_httpclient::TypePostArgument>(nType);
}

BOOL wininet_httpclient::Connect(LPCTSTR szAddress, LPCTSTR szAgent, unsigned short nPort, LPCTSTR szUserAccount, LPCTSTR szPassword){

	_hHTTPOpen=::InternetOpen(szAgent,												// agent name
												INTERNET_OPEN_TYPE_PRECONFIG,	// proxy option
												_T(""),														// proxy
												_T(""),												// proxy bypass
												0);					// flags

	if(!_hHTTPOpen){
		_dwError=::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet=FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
														  NULL,
														  ::GetLastError(),
														  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
														  reinterpret_cast<LPTSTR>(&lpMsgBuffer),
														  0,
														  NULL);
		OutputDebugString(reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}	

	_hHTTPConnection=::InternetConnect(	_hHTTPOpen,	// internet opened handle
															szAddress, // server name
															nPort, // ports
															szUserAccount, // user name
															szPassword, // password 
															INTERNET_SERVICE_HTTP, // service type
															INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE,	// service option																														
															0);	// context call-back option

	if(!_hHTTPConnection){		
		_dwError=::GetLastError();
		::CloseHandle(_hHTTPOpen);
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet=FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
														  NULL,
														  ::GetLastError(),
														  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
														  reinterpret_cast<LPTSTR>(&lpMsgBuffer),
														  0,
														  NULL);
		OutputDebugString(reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

	if(::InternetAttemptConnect(NULL)!=ERROR_SUCCESS){		
		_dwError=::GetLastError();
		::CloseHandle(_hHTTPConnection);
		::CloseHandle(_hHTTPOpen);
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet=FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
														  NULL,
														  ::GetLastError(),
														  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
														  reinterpret_cast<LPTSTR>(&lpMsgBuffer),
														  0,
														  NULL);
		OutputDebugString(reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

	return TRUE;															
}

BOOL wininet_httpclient::Close(){

	if(_hHTTPRequest)
		::InternetCloseHandle(_hHTTPRequest);

	if(_hHTTPConnection)
		::InternetCloseHandle(_hHTTPConnection);

	if(_hHTTPOpen)
		::InternetCloseHandle(_hHTTPOpen);

	return TRUE;
}

BOOL wininet_httpclient::Request(LPCTSTR szURL, int nMethod, LPCTSTR szAgent){

	BOOL bReturn=TRUE;
	DWORD dwPort=0;
	TCHAR szProtocol[__SIZE_BUFFER] = _T("");
	TCHAR szAddress[__SIZE_BUFFER]  = _T("");	
	TCHAR szURI[__SIZE_BUFFER]      = _T("");
	DWORD dwSize=0;

	ParseURL(szURL, szProtocol, szAddress, dwPort, szURI);

	if(Connect(szAddress, szAgent, dwPort)){
		if(!RequestOfURI(szURI, nMethod)){
			bReturn=FALSE;
		}else{
			if(!Response((PBYTE)_szHTTPResponseHeader, __SIZE_HTTP_BUFFER, (PBYTE)_szHTTPResponseHTML, __SIZE_HTTP_BUFFER, dwSize)){
				bReturn=FALSE;		
			}
		}
		Close();
	}else{
		bReturn=FALSE;
	}

	return bReturn;
}

BOOL wininet_httpclient::RequestOfURI(LPCTSTR szURI, int nMethod){

	BOOL bReturn=TRUE;

	try{
		switch(nMethod){
		case	wininet_httpclient::RequestGetMethod:
		default:
			bReturn=RequestGet(szURI);
			break;
		case	wininet_httpclient::RequestPostMethod:		
			bReturn=RequestPost(szURI);
			break;
		case	wininet_httpclient::RequestPostMethodMultiPartsFormData:
			bReturn=RequestPostMultiPartsFormData(szURI);
			break;
		}
	}catch(...){
#ifdef	_DEBUG
#endif
	}
	

	return bReturn;
}

BOOL wininet_httpclient::RequestGet(LPCTSTR szURI){

	CONST TCHAR *szAcceptType = __HTTP_ACCEPT_TYPE;

	_hHTTPRequest=::HttpOpenRequest(	_hHTTPConnection,
															__HTTP_VERB_GET, // HTTP Verb
															szURI, // Object Name
															HTTP_VERSION, // Version
															_T(""), // Reference
															&szAcceptType, // Accept Type
															INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE,
															0); // context call-back point

	if(!_hHTTPRequest){
		_dwError=::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet=FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
														  NULL,
														  ::GetLastError(),
														  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
														  reinterpret_cast<LPTSTR>(&lpMsgBuffer),
														  0,
														  NULL);
		OutputDebugString(reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

	// REPLACE HEADER
	if(!::HttpAddRequestHeaders( _hHTTPRequest, __HTTP_ACCEPT, _tcslen(__HTTP_ACCEPT), HTTP_ADDREQ_FLAG_REPLACE)){
		_dwError=::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet=FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
														  NULL,
														  ::GetLastError(),
														  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
														  reinterpret_cast<LPTSTR>(&lpMsgBuffer),
														  0,
														  NULL);
		OutputDebugString(reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

	// SEND REQUEST
	if(!::HttpSendRequest( _hHTTPRequest,	// handle by returned HttpOpenRequest
									NULL, // additional HTTP header
									0, // additional HTTP header length
									NULL, // additional data in HTTP Post or HTTP Put
									0) // additional data length
									){
		_dwError=::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet=FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
														  NULL,
														  ::GetLastError(),
														  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
														  reinterpret_cast<LPTSTR>(&lpMsgBuffer),
														  0,
														  NULL);
		OutputDebugString(reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

	return TRUE;
}

BOOL wininet_httpclient::RequestPost(LPCTSTR szURI){

	CONST TCHAR *szAcceptType   =   __HTTP_ACCEPT_TYPE;
	TCHAR	szPostArguments[__SIZE_BUFFER]  = _T("");
	LPCTSTR szContentType = TEXT("Content-Type: application/x-www-form-urlencoded\r\n");

	DWORD dwPostArgumentsLegnth=GetPostArguments(szPostArguments, __SIZE_BUFFER);
	
	_hHTTPRequest=::HttpOpenRequest(	_hHTTPConnection,
															__HTTP_VERB_POST, // HTTP Verb
															szURI, // Object Name
															HTTP_VERSION, // Version
															_T(""), // Reference
															&szAcceptType, // Accept Type
															INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_FORMS_SUBMIT,
															0); // context call-back point

	if(!_hHTTPRequest){
		_dwError=::GetLastError();
    InternetCloseHandle(_hHTTPRequest);
		return FALSE;
	}

	// REPLACE HEADER
	if(!::HttpAddRequestHeaders( _hHTTPRequest, __HTTP_ACCEPT, _tcslen(__HTTP_ACCEPT), HTTP_ADDREQ_FLAG_REPLACE)){
		_dwError=::GetLastError();
    InternetCloseHandle(_hHTTPRequest);
		return FALSE;
	}


	// SEND REQUEST
	if(!::HttpSendRequest( _hHTTPRequest,	// handle by returned HttpOpenRequest
									szContentType, // additional HTTP header
									_tcslen(szContentType), // additional HTTP header length
									(LPVOID)szPostArguments, // additional data in HTTP Post or HTTP Put
									_tcslen(szPostArguments)) // additional data length
									){
		_dwError=::GetLastError();
    InternetCloseHandle(_hHTTPRequest);
		return FALSE;
	}

  InternetCloseHandle(_hHTTPRequest);
	return TRUE;
}

BOOL wininet_httpclient::RequestPostMultiPartsFormData(LPCTSTR szURI){

	CONST TCHAR *szAcceptType = __HTTP_ACCEPT_TYPE;	
	LPCTSTR szContentType = TEXT("Content-Type: multipart/form-data; boundary=--MULTI-PARTS-FORM-DATA-BOUNDARY\r\n");		
	
	// ALLOCATE POST MULTI-PARTS FORM DATA ARGUMENTS
	PBYTE pPostBuffer=NULL;
	DWORD dwPostBufferLength=AllocMultiPartsFormData(pPostBuffer, _T("--MULTI-PARTS-FORM-DATA-BOUNDARY"));

	_hHTTPRequest=::HttpOpenRequest(	_hHTTPConnection,
															__HTTP_VERB_POST, // HTTP Verb
															szURI, // Object Name
															HTTP_VERSION, // Version
															_T(""), // Reference
															&szAcceptType, // Accept Type
															INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_FORMS_SUBMIT,	// flags
															0); // context call-back point
	if(!_hHTTPRequest){
		_dwError=::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet=FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
														  NULL,
														  ::GetLastError(),
														  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
														  reinterpret_cast<LPTSTR>(&lpMsgBuffer),
														  0,
														  NULL);
		OutputDebugString(reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

	// REPLACE HEADER
	if(!::HttpAddRequestHeaders( _hHTTPRequest, __HTTP_ACCEPT, _tcslen(__HTTP_ACCEPT), HTTP_ADDREQ_FLAG_REPLACE)){
		_dwError=::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet=FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
														  NULL,
														  ::GetLastError(),
														  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
														  reinterpret_cast<LPTSTR>(&lpMsgBuffer),
														  0,
														  NULL);
		//TRACE(reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

	if(!::HttpAddRequestHeaders( _hHTTPRequest, szContentType, _tcslen(szContentType), HTTP_ADDREQ_FLAG_ADD_IF_NEW)){
		_dwError=::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet=FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
														  NULL,
														  ::GetLastError(),
														  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
														  reinterpret_cast<LPTSTR>(&lpMsgBuffer),
														  0,
														  NULL);
		//TRACE(reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

	TCHAR	szContentLength[__SIZE_BUFFER]  = _T("");
	::ZeroMemory(szContentLength, __SIZE_BUFFER);

	_stprintf( szContentLength, _T("Content-Length: %d\r\n"), dwPostBufferLength);

	if(!::HttpAddRequestHeaders( _hHTTPRequest, szContentLength, _tcslen(szContentLength), HTTP_ADDREQ_FLAG_ADD_IF_NEW)){
		_dwError=::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet=FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
														  NULL,
														  ::GetLastError(),
														  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
														  reinterpret_cast<LPTSTR>(&lpMsgBuffer),
														  0,
														  NULL);
		//TRACE(reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

#ifdef	_DEBUG
	TCHAR	szHTTPRequest[__SIZE_HTTP_BUFFER] = _T("");
	DWORD	dwHTTPRequestLength = __SIZE_HTTP_BUFFER;

	::ZeroMemory(szHTTPRequest, __SIZE_HTTP_BUFFER);
	if(!::HttpQueryInfo(_hHTTPRequest, HTTP_QUERY_RAW_HEADERS_CRLF, szHTTPRequest, &dwHTTPRequestLength, NULL)){
		_dwError=::GetLastError();
		LPVOID     lpMsgBuffer;
		DWORD dwRet=FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
														  NULL,
														  ::GetLastError(),
														  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
														  reinterpret_cast<LPTSTR>(&lpMsgBuffer),
														  0,
														  NULL);
		//TRACE("\n%d : %s\n", ::GetLastError(), reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
		//return FALSE;
	}

	//TRACE(szHTTPRequest);
#endif

	// SEND REQUEST WITH HttpSendRequestEx and InternetWriteFile
	INTERNET_BUFFERS InternetBufferIn={0};
	InternetBufferIn.dwStructSize=sizeof(INTERNET_BUFFERS);
	InternetBufferIn.Next=NULL;	
	
	if(!::HttpSendRequestEx(_hHTTPRequest, &InternetBufferIn, NULL, HSR_INITIATE, 0)){
		_dwError=::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet=FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
														  NULL,
														  ::GetLastError(),
														  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
														  reinterpret_cast<LPTSTR>(&lpMsgBuffer),
														  0,
														  NULL);
		//TRACE(reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

	DWORD dwOutPostBufferLength=0;
	if(!::InternetWriteFile(_hHTTPRequest, pPostBuffer, dwPostBufferLength, &dwOutPostBufferLength)){
		_dwError=::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet=FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
														  NULL,
														  ::GetLastError(),
														  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
														  reinterpret_cast<LPTSTR>(&lpMsgBuffer),
														  0,
														  NULL);
		OutputDebugString(reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

	if(!::HttpEndRequest(_hHTTPRequest, NULL, HSR_INITIATE, 0)){
		_dwError=::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet=FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
														  NULL,
														  ::GetLastError(),
														  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
														  reinterpret_cast<LPTSTR>(&lpMsgBuffer),
														  0,
														  NULL);
		OutputDebugString(reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

	// FREE POST MULTI-PARTS FORM DATA ARGUMENTS
	FreeMultiPartsFormData(pPostBuffer);

	return TRUE;
}

DWORD wininet_httpclient::ResponseOfBytes(PBYTE pBuffer, DWORD dwSize){

	static DWORD dwBytes=0;

	if(!_hHTTPRequest){
		_dwError=::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet=FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
														  NULL,
														  ::GetLastError(),
														  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
														  reinterpret_cast<LPTSTR>(&lpMsgBuffer),
														  0,
														  NULL);
		OutputDebugString(reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return 0;
	}

	::ZeroMemory(pBuffer, dwSize);
	if(!::InternetReadFile(	_hHTTPRequest,
									pBuffer,
									dwSize,
									&dwBytes)){
		_dwError=::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet=FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
														  NULL,
														  ::GetLastError(),
														  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
														  reinterpret_cast<LPTSTR>(&lpMsgBuffer),
														  0,
														  NULL);
		OutputDebugString(reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return 0;
	}

	return dwBytes;
}

BOOL wininet_httpclient::Response(PBYTE pHeaderBuffer, DWORD dwHeaderBufferLength, PBYTE pBuffer, DWORD dwBufferLength, DWORD &dwResultSize){

	BYTE pResponseBuffer[__SIZE_BUFFER]="";	
	DWORD dwNumOfBytesToRead=0;

	if(!_hHTTPRequest){
		_dwError=::GetLastError();
#ifdef	_DEBUG		
		LPVOID     lpMsgBuffer;
		DWORD dwRet=FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
														  NULL,
														  ::GetLastError(),
														  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
														  reinterpret_cast<LPTSTR>(&lpMsgBuffer),
														  0,
														  NULL);
		OutputDebugString(reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

	::ZeroMemory(pBuffer, dwBufferLength);
	dwResultSize=0;
	while((dwNumOfBytesToRead=ResponseOfBytes(pResponseBuffer, __SIZE_BUFFER))!=NULL && dwResultSize<dwBufferLength){
		::CopyMemory( (pBuffer+dwResultSize), pResponseBuffer, dwNumOfBytesToRead);		
		dwResultSize+=dwNumOfBytesToRead;
	}

	::ZeroMemory(pHeaderBuffer, dwHeaderBufferLength);
	if(!::HttpQueryInfo(_hHTTPRequest, HTTP_QUERY_RAW_HEADERS_CRLF, pHeaderBuffer, &dwHeaderBufferLength, NULL)){
		_dwError=::GetLastError();
#ifdef	_DEBUG
		LPVOID     lpMsgBuffer;
		DWORD dwRet=FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
														  NULL,
														  ::GetLastError(),
														  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
														  reinterpret_cast<LPTSTR>(&lpMsgBuffer),
														  0,
														  NULL);
		OutputDebugString(reinterpret_cast<LPTSTR>(lpMsgBuffer));
		LocalFree(lpMsgBuffer);		
#endif
		return FALSE;
	}

	return (dwResultSize? TRUE: FALSE);
}

VOID wininet_httpclient::AddPostArguments(LPCTSTR szName, LPCTSTR szValue, BOOL bBinary){

	GenericHTTPArgument	arg;
	::ZeroMemory(&arg, sizeof(arg));

	_tcscpy(arg.szName, szName);
	_tcscpy(arg.szValue, szValue);

	if(!bBinary)
		arg.dwType = wininet_httpclient::TypeNormal;
	else
		arg.dwType = wininet_httpclient::TypeBinary;	

	_vArguments.push_back(arg);

	return;
}

VOID wininet_httpclient::AddPostArguments(LPCTSTR szName, DWORD nValue){

	GenericHTTPArgument	arg;
	::FillMemory(&arg, sizeof(arg), 0x00);

	_tcscpy(arg.szName, szName);
	_stprintf(arg.szValue, _T("%d"), nValue);
	arg.dwType = wininet_httpclient::TypeNormal;

	_vArguments.push_back(arg);

	return;
}

DWORD wininet_httpclient::GetPostArguments(LPTSTR szArguments, DWORD dwLength){

	std::vector<GenericHTTPArgument>::iterator itArg;

	::ZeroMemory(szArguments, dwLength);
	for(itArg=_vArguments.begin(); itArg<_vArguments.end(); ){
		_tcscat(szArguments, itArg->szName);
		_tcscat(szArguments, _T("="));
		_tcscat(szArguments, itArg->szValue);

		if((++itArg)<_vArguments.end()){
			_tcscat(szArguments, _T("&"));
		}
	}

	*(szArguments + dwLength) = _T('\0');

	return _tcslen(szArguments);
}


VOID wininet_httpclient::InitilizePostArguments(){
	_vArguments.clear();
	return;
}

VOID wininet_httpclient::FreeMultiPartsFormData(PBYTE &pBuffer){

	if(pBuffer){
		::LocalFree(pBuffer);
		pBuffer=NULL;
	}

	return;
}

DWORD wininet_httpclient::AllocMultiPartsFormData(PBYTE &pInBuffer, LPCTSTR szBoundary){

	if(pInBuffer){
		::LocalFree(pInBuffer);
		pInBuffer=NULL;
	}

	pInBuffer=(PBYTE)::LocalAlloc(LPTR, GetMultiPartsFormDataLength());	
	std::vector<GenericHTTPArgument>::iterator itArgv;

	DWORD dwPosition=0;
	DWORD dwBufferSize=0;

	for(itArgv=_vArguments.begin(); itArgv<_vArguments.end(); ++itArgv){

		PBYTE pBuffer=NULL;

		// SET MULTI_PRATS FORM DATA BUFFER
		switch(itArgv->dwType){
		case	wininet_httpclient::TypeNormal:
			pBuffer=(PBYTE)::LocalAlloc(LPTR, __SIZE_HTTP_HEAD_LINE*4);

			_stprintf(	(TCHAR*)pBuffer,							
							_T("--%s\r\n")
							_T("Content-Disposition: form-data; name=\"%s\"\r\n")
							_T("\r\n")
							_T("%s\r\n"),
							szBoundary,
							itArgv->szName,
							itArgv->szValue);

			dwBufferSize=_tcslen((TCHAR*)pBuffer);

			break;
		case	wininet_httpclient::TypeBinary:
			DWORD	dwNumOfBytesToRead=0;
			DWORD	dwTotalBytes=0;

			HANDLE hFile=::CreateFile(itArgv->szValue, 
									GENERIC_READ, // desired access
									FILE_SHARE_READ, // share mode
									NULL, // security attribute
									OPEN_EXISTING, // create disposition
									FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, // flag and attributes
									NULL); // template file handle

			DWORD	dwSize=::GetFileSize(hFile, NULL);

			pBuffer=(PBYTE)::LocalAlloc(LPTR, __SIZE_HTTP_HEAD_LINE*3+dwSize+1);
			BYTE	pBytes[__SIZE_BUFFER+1]="";
			
			_stprintf( (TCHAR*)pBuffer,
							_T("--%s\r\n")
							_T("Content-Disposition: form-data; name=\"%s\"; filename=\"%s\"\r\n")
							_T("Content-Type: %s\r\n")
							_T("\r\n"),
							szBoundary,
							itArgv->szName, itArgv->szValue,
							GetContentType(itArgv->szValue)
							);

			DWORD	dwBufPosition=_tcslen((TCHAR*)pBuffer);	
	
			while(::ReadFile(hFile, pBytes, __SIZE_BUFFER, &dwNumOfBytesToRead, NULL) && dwNumOfBytesToRead>0 && dwTotalBytes<=dwSize){
				::CopyMemory((pBuffer+dwBufPosition+dwTotalBytes), pBytes, dwNumOfBytesToRead);
				::ZeroMemory(pBytes, __SIZE_BUFFER+1);
				dwTotalBytes += dwNumOfBytesToRead;				
			}

			::CopyMemory((pBuffer+dwBufPosition+dwTotalBytes), _T("\r\n"), _tcslen(_T("\r\n")));
			::CloseHandle(hFile);

			dwBufferSize=dwBufPosition+dwTotalBytes + _tcslen(_T("\r\n"));
			break;			
		}

		::CopyMemory((pInBuffer+dwPosition), pBuffer, dwBufferSize);
		dwPosition+=dwBufferSize;

		if(pBuffer)
        {
			::LocalFree(pBuffer);
			pBuffer=NULL;
		}
	}

	::CopyMemory((pInBuffer+dwPosition), _T("--"), 2);
	::CopyMemory((pInBuffer+dwPosition+2), szBoundary, _tcslen(szBoundary));
	::CopyMemory((pInBuffer+dwPosition+2+_tcslen(szBoundary)), _T("--\r\n"), 3);

	return dwPosition+5+_tcslen(szBoundary);
}

DWORD wininet_httpclient::GetMultiPartsFormDataLength(){
	std::vector<GenericHTTPArgument>::iterator itArgv;

	DWORD	dwLength=0;

	for(itArgv=_vArguments.begin(); itArgv<_vArguments.end(); ++itArgv){

		switch(itArgv->dwType){
		case	wininet_httpclient::TypeNormal:
			dwLength+=__SIZE_HTTP_HEAD_LINE*4;
			break;
		case	wininet_httpclient::TypeBinary:
			HANDLE hFile=::CreateFile(itArgv->szValue, 
									GENERIC_READ, // desired access
									FILE_SHARE_READ, // share mode
									NULL, // security attribute
									OPEN_EXISTING, // create disposition
									FILE_ATTRIBUTE_NORMAL, // flag and attributes
									NULL); // template file handle

			dwLength += __SIZE_HTTP_HEAD_LINE*3+::GetFileSize(hFile, NULL);
			::CloseHandle(hFile);			
			break;
		}

	}

	return dwLength;
}

LPCTSTR wininet_httpclient::GetContentType(LPCTSTR szName){

	static TCHAR	szReturn[1024] = _T("");
	LONG	dwLen=1024;
	DWORD	dwDot=0;

	for(dwDot=_tcslen(szName);dwDot>0;dwDot--)
    {
		if(!_tcsncmp((szName+dwDot), _T("."), 1))
			break;
	}

	HKEY	hKEY;
	LPTSTR	szWord = (TCHAR*)(szName + dwDot);

	if(RegOpenKeyEx(HKEY_CLASSES_ROOT, szWord, 0, KEY_QUERY_VALUE, &hKEY)==ERROR_SUCCESS){
		if(RegQueryValueEx(hKEY, TEXT("Content Type"), NULL, NULL, (LPBYTE)szReturn, (unsigned long*)&dwLen)!=ERROR_SUCCESS)
			_tcsncpy(szReturn, _T("application/octet-stream"), _tcslen(_T("application/octet-stream")));
		RegCloseKey(hKEY);
	}else{
		_tcsncpy(szReturn, _T("application/octet-stream"), _tcslen(_T("application/octet-stream")));
	}

	return szReturn;
}

DWORD wininet_httpclient::GetLastError(){

	return _dwError;
}

VOID wininet_httpclient::ParseURL(LPCTSTR szURL, LPTSTR szProtocol, LPTSTR szAddress, DWORD &dwPort, LPTSTR szURI){

	TCHAR szPort[__SIZE_BUFFER] = _T("");
	DWORD dwPosition=0;
	BOOL bFlag=FALSE;

	while(_tcslen(szURL)>0 && dwPosition<_tcslen(szURL) && _tcsncmp((szURL+dwPosition), _T(":"), 1))
		++dwPosition;

	if(!_tcsncmp((szURL+dwPosition+1), _T("/"), 1)){	// is PROTOCOL
		if(szProtocol){
			_tcsncpy(szProtocol, szURL, dwPosition);
			szProtocol[dwPosition]=0;
		}
		bFlag=TRUE;
	}else{	// is HOST 
		if(szProtocol){
			_tcsncpy(szProtocol, _T("http"), 4);
			szProtocol[5]=0;
		}
	}

	DWORD dwStartPosition=0;
	
	if(bFlag)
    {
		dwStartPosition=dwPosition+=3;				
	}
    else
    {
		dwStartPosition=dwPosition=0;
	}

	bFlag=FALSE;
	while(_tcslen(szURL)>0 && dwPosition<_tcslen(szURL) && _tcsncmp((szURL+dwPosition), _T("/"), 1))
			++dwPosition;

	DWORD dwFind=dwStartPosition;

	for(;dwFind<=dwPosition;dwFind++){
		if(!_tcsncmp((szURL+dwFind), _T(":"), 1)){ // find PORT
			bFlag=TRUE;
			break;
		}
	}

	if(bFlag){
		TCHAR sztmp[__SIZE_BUFFER] = _T("");
		_tcsncpy(sztmp, (szURL + dwFind+1), dwPosition-dwFind);
		dwPort=_ttol(sztmp);
		_tcsncpy(szAddress, (szURL + dwStartPosition), dwFind-dwStartPosition);
	}else if(!_tcscmp(szProtocol, _T("https"))){
		dwPort=INTERNET_DEFAULT_HTTPS_PORT;
		_tcsncpy(szAddress, (szURL + dwStartPosition), dwPosition-dwStartPosition);
	}else {
		dwPort=INTERNET_DEFAULT_HTTP_PORT;
		_tcsncpy(szAddress, (szURL + dwStartPosition), dwPosition-dwStartPosition);
	}

	if(dwPosition<_tcslen(szURL))
    { // find URI
		_tcsncpy(szURI, (szURL + dwPosition), _tcslen(szURL)-dwPosition);
	}else
    {
		szURI[0]=0;
	}

	return;
}

LPCTSTR wininet_httpclient::QueryHTTPResponseHeader(){
	return _szHTTPResponseHeader;
}

LPCTSTR wininet_httpclient::QueryHTTPResponse(){
	return _szHTTPResponseHTML;
}


