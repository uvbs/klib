#include "mac_extractor.h"


#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>


#pragma comment(lib,"netapi32.lib")


typedef struct _ASTAT_
{
    ADAPTER_STATUS adapt;
    NAME_BUFFER NameBuff [30];
} ASTAT, *PASTAT;

mac_extractor::mac_extractor(void)
{
    extract_mac_lst();
}

mac_extractor::~mac_extractor(void)
{
}

const mac_info_list&  mac_extractor::get_mac_list()
{
    return mac_lst_;
}

void mac_extractor::extract_mac_lst()
{
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
            minfo.name_ = std::string((char*)adpt.NameBuff->name, adpt.NameBuff->name_num);
            mac_lst_.push_back(std::move(minfo));
        }
    }
}