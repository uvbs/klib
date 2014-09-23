#include "mac_extractor.h"


#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#pragma comment(lib,"netapi32.lib")


#include <iphlpapi.h>
# pragma comment(lib,"Iphlpapi.lib")


mac_extractor::mac_extractor(void)
{
}

mac_extractor::~mac_extractor(void)
{
}

const mac_info_list&  mac_extractor::get_mac_list()
{
    return mac_lst_;
}

void mac_extractor::extract_mac_lst(extract_mac_method method)
{
    if (e_extract_helpapi == method) {
        extract_mac_lst_helpapi();
    }
    else if (e_extract_netbios == method) {
        extract_mac_lst_netbios();
    }
}

void mac_extractor::extract_mac_lst_netbios()
{
    typedef struct _ASTAT_
    {
        ADAPTER_STATUS adapt;
        NAME_BUFFER NameBuff [30];
    } ASTAT, *PASTAT;

    ASTAT adpt;
    NCB Ncb;
    UCHAR uRetCode;
    LANA_ENUM lenum;

    memset(&Ncb, 0, sizeof(Ncb));
    Ncb.ncb_command = NCBENUM;
    Ncb.ncb_buffer  = (UCHAR *)&lenum;
    Ncb.ncb_length  = sizeof(lenum);

    uRetCode = Netbios( &Ncb );
    for(int index=0; index < lenum.length ; index++)
    {
        memset(&Ncb, 0, sizeof(Ncb));
        Ncb.ncb_command  = NCBRESET;
        Ncb.ncb_lana_num = lenum.lana[index];
        uRetCode = Netbios( &Ncb );

        memset(&Ncb, 0, sizeof(Ncb));
        Ncb.ncb_command = NCBASTAT;
        Ncb.ncb_lana_num = lenum.lana[index];
        strcpy((char *)Ncb.ncb_callname, "* ");
        Ncb.ncb_buffer = (unsigned char *) &adpt;
        Ncb.ncb_length = sizeof(adpt);
        uRetCode = Netbios( &Ncb );

        mac_info minfo;
        char mac_buf[30] = {0};
        if (uRetCode == 0)
        {
            int ret = sprintf(mac_buf, "%02X%02X%02X%02X%02X%02X",
                adpt.adapt.adapter_address[0],
                adpt.adapt.adapter_address[1],
                adpt.adapt.adapter_address[2],
                adpt.adapt.adapter_address[3],
                adpt.adapt.adapter_address[4],
                adpt.adapt.adapter_address[5]
            );
            if (ret < 0) {
                continue;
            }

            minfo.mac_ = std::string(mac_buf, ret);
            //minfo.name_ = std::string((char*)adpt.NameBuff->name, adpt.NameBuff->name_num);
            mac_lst_.push_back(std::move(minfo));
        }
    }
}

void mac_extractor::extract_mac_lst_helpapi()
{
    PIP_ADAPTER_INFO pAdapterInfo;
    PIP_ADAPTER_INFO pAdapter = NULL;
    DWORD dwRetVal = 0;
    pAdapterInfo   = (IP_ADAPTER_INFO*) malloc(sizeof(IP_ADAPTER_INFO));
    ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);

    if(GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) != ERROR_SUCCESS){
        GlobalFree(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO*) malloc(ulOutBufLen);
    }

    if((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR)
    {
        pAdapter = pAdapterInfo;
        mac_lst_.clear();
        while (pAdapter)
        {
            //pAdapter->Type是71为：无线网卡
            //pAdapter->Description中包含"PCI"为：物理网卡

            /*if(strstr(pAdapter->Description,"PCI")>0 ||pAdapter->Type==71){
            printf("------------------------------------------------------------\n");
            printf("AdapterName:\t%s\n",pAdapter->AdapterName);
            printf("AdapterDesc:\t%s\n",pAdapter->Description);
            printf("AdapterAddr:\t");
            for(UINT i=0;i<pAdapter->AddressLength;i++){
                printf("%02X%c",pAdapter->Address[i],
                    i == pAdapter->AddressLength-1? '\n':'-');
            }

            printf("AdapterType:\t%d\n",pAdapter->Type);
            printf("IPAddress:\t%s\n",pAdapter->IpAddressList.IpAddress.String);
            printf("IPMask:\t%s\n",pAdapter->IpAddressList.IpMask.String);
            */

            mac_info minfo;
            char mac_buf[30] = {0};
            int ret = sprintf(mac_buf, "%02X%02X%02X%02X%02X%02X",
                pAdapter->Address[0],
                pAdapter->Address[1],
                pAdapter->Address[2],
                pAdapter->Address[3],
                pAdapter->Address[4],
                pAdapter->Address[5]
            );
            if (ret < 0) {
                continue;
            }

            minfo.type_ = pAdapter->Type;
            minfo.mac_  = std::string(mac_buf, ret);
            minfo.name_ = pAdapter->Description;
            mac_lst_.push_back(std::move(minfo));

            pAdapter=pAdapter->Next;
        }
    }
    else
    {
        printf("Call to Get Adapters Information failed\n"); 
    }

    return ;
}