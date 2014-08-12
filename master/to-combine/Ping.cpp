#include <winsock.h>
#include <stdio.h>
#include "ping.h"
#pragma hdrstop
// Download by http://www.codefans.net
//---------------------------------------------------------------------------
#pragma package(smart_init)

#define MIN_ICMP_PACKET_SIZE 8    //minimum 8 byte icmp packet (just header)
#define MAX_ICMP_PACKET_SIZE 1024 //Maximum icmp packet size

// IP header
typedef struct tagIP_HEADER
{
	unsigned int h_len:4;          // length of the header
	unsigned int version:4;        // Version of IP
	unsigned char tos;             // Type of service
	unsigned short total_len;      // total length of the packet
	unsigned short ident;          // unique identifier
	unsigned short frag_and_flags; // flags
	unsigned char ttl;
	unsigned char proto;           // protocol (TCP, UDP etc)
	unsigned short checksum;       // IP checksum
	unsigned int sourceIP;
	unsigned int destIP;
} IP_HEADER;
typedef IP_HEADER FAR* LPIP_HEADER;

// ICMP header
typedef struct tagICMP_HEADER
{
	BYTE i_type;
	BYTE i_code; /* type sub code */
	USHORT i_cksum;
	USHORT i_id;
	USHORT i_seq;
	/* This is not the std header, but we reserve space for time */
	ULONG timestamp;
} ICMP_HEADER;
typedef ICMP_HEADER FAR* LPICMP_HEADER;

void FillIcmpData(LPICMP_HEADER pIcmp, int nData);
BOOL DecodeResponse(char* pBuf, int nBytes, sockaddr_in* from);
USHORT GenerateIPChecksum(USHORT* pBuffer, int nSize);


/////////////////////////////////  Macros & Statics ///////////////////////////

#ifdef _DEBUG
//#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL CPing::sm_bAttemptedIcmpInitialise = FALSE;
lpIcmpCreateFile CPing::sm_pIcmpCreateFile = NULL;
lpIcmpSendEcho CPing::sm_pIcmpSendEcho = NULL;
lpIcmpCloseHandle CPing::sm_pIcmpCloseHandle = NULL;

__int64 CPing::sm_TimerFrequency = 0;


//Internal class which is used to ensure that the ICMP
//handle and winsock stack is closed upon exit
class _CPING
{
public:
  _CPING();
	~_CPING();
protected:
  HINSTANCE sm_hIcmp;

	friend class CPing;
};

_CPING::_CPING()
{
	sm_hIcmp = NULL;
}

_CPING::~_CPING()
{
	if (sm_hIcmp) {
 		FreeLibrary(sm_hIcmp);
	 	sm_hIcmp = NULL;
	}
	WSACleanup();
}

static _CPING _cpingData;




///////////////////////////////// Implementation //////////////////////////////


BOOL CPing::Initialise() const
{
	if (!sm_bAttemptedIcmpInitialise) {
		sm_bAttemptedIcmpInitialise = TRUE;

		//Initialise the winsock stack
		WSADATA wsa;
		if (WSAStartup(MAKEWORD(1, 1), &wsa) != 0) {
			::MessageBox(NULL,"WinSock版本不匹配","ERROR",MB_OK);
			return FALSE;
		}

		//Load up the ICMP library
		_cpingData.sm_hIcmp = LoadLibrary("ICMP.DLL");
		if (_cpingData.sm_hIcmp == NULL) {
			::MessageBox(NULL,"无法载入'ICMP.DLL'","ERROR",MB_OK);
			return FALSE;
		}

		//Retrieve pointers to the functions in the ICMP dll
		sm_pIcmpCreateFile = (lpIcmpCreateFile) GetProcAddress(_cpingData.sm_hIcmp,"IcmpCreateFile");
		sm_pIcmpSendEcho = (lpIcmpSendEcho) GetProcAddress(_cpingData.sm_hIcmp,"IcmpSendEcho" );
		sm_pIcmpCloseHandle = (lpIcmpCloseHandle) GetProcAddress(_cpingData.sm_hIcmp,"IcmpCloseHandle");
		if (sm_pIcmpCreateFile == NULL || sm_pIcmpSendEcho == NULL ||
			sm_pIcmpCloseHandle == NULL)
			::MessageBox(NULL,"'ICMP.DLL'中函数无效","ERROR",MB_OK);
	}

	return (sm_pIcmpCreateFile != NULL && sm_pIcmpSendEcho != NULL &&
                sm_pIcmpCloseHandle != NULL);
}


BOOL CPing::IsSocketReadible(SOCKET socket, DWORD dwTimeout, BOOL& bReadible)
{
	timeval timeout = {dwTimeout/1000, dwTimeout % 1000};
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(socket, &fds);
	int nStatus = select(0, &fds, NULL, NULL, &timeout);
	if (nStatus == SOCKET_ERROR) {
		return FALSE;
	}
	else {
		bReadible = !(nStatus == 0);
		return TRUE;
	}
}

BOOL CPing::Ping(LPCTSTR pszHostName, CPingReply& pr, UCHAR nTTL, DWORD dwTimeout, UCHAR nPacketSize) const
{
	//Make sure everything is initialised
	if (!Initialise())
	  return FALSE;

	LPSTR lpszAscii = (LPTSTR) pszHostName;
	//Convert from dotted notation if required
	unsigned long	addr = inet_addr(lpszAscii);
	if (addr == INADDR_NONE) {
		//Not a dotted address, then do a lookup of the name
		hostent* hp = gethostbyname(lpszAscii);
		if (hp)
			memcpy(&addr, hp->h_addr, hp->h_length);
		else {
      char msg[64];
      sprintf(msg,"无法解析主机名：%s",pszHostName);
			::MessageBox(NULL,msg,"ERROR",MB_OK);
			return FALSE;
		}
	}

	//Create the ICMP handle
	HANDLE hIP = sm_pIcmpCreateFile();
	if (hIP == INVALID_HANDLE_VALUE) {
		::MessageBox(NULL,"无效的'ICMP'句柄","ERROR",MB_OK);
		return FALSE;
	}

	//Set up the option info structure
	IP_OPTION_INFORMATION OptionInfo;
	ZeroMemory(&OptionInfo, sizeof(IP_OPTION_INFORMATION));
	OptionInfo.Ttl = nTTL;

	//Set up the data which will be sent
	unsigned char* pBuf = new unsigned char[nPacketSize];
	memset(pBuf, 'E', nPacketSize);

	//Do the actual Ping
	int nReplySize = sizeof(ICMP_ECHO_REPLY) + max(MIN_ICMP_PACKET_SIZE, nPacketSize);
	unsigned char* pReply = new unsigned char[nReplySize];
	ICMP_ECHO_REPLY* pEchoReply = (ICMP_ECHO_REPLY*) pReply;
	DWORD nRecvPackets = sm_pIcmpSendEcho(hIP, addr, pBuf, nPacketSize, &OptionInfo, pReply, nReplySize, dwTimeout);

	//Check we got the packet back
	BOOL bSuccess = (nRecvPackets == 1);

	//Check the IP status is OK (O is IP Success)
	if (bSuccess && (pEchoReply->Status != 0)) {
		bSuccess = FALSE;
		SetLastError(pEchoReply->Status);
	}

	//Check we got the same amount of data back as we sent
	if (bSuccess) {
		bSuccess = (pEchoReply->DataSize == nPacketSize);
		if (!bSuccess)
			SetLastError(ERROR_UNEXP_NET_ERR);
	}

	//Check the data we got back is what was sent
	if (bSuccess) {
		char* pReplyData = (char*) pEchoReply->Data;
		for (int i=0; i<nPacketSize && bSuccess; i++)
			bSuccess = (pReplyData[i] == 'E');

		if (!bSuccess)
			SetLastError(ERROR_UNEXP_NET_ERR);
	}

	//Close the ICMP handle
	sm_pIcmpCloseHandle(hIP);

	if (bSuccess) {
		//Ping was successful, copy over the pertinent info
		//into the return structure
		pr.Address.S_un.S_addr = pEchoReply->Address;
		pr.RTT = pEchoReply->RoundTripTime;
	}

	//Free up the memory we allocated
	delete [] pBuf;
	delete [] pReply;

	//return the status
	return bSuccess;
}



