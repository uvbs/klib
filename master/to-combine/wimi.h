#ifndef _WMI_H_
#define _WMI_H_

#define _WIN32_DCOM
#include <comdef.h>
#include <Wbemidl.h>
#pragma comment(lib, "wbemuuid.lib")

#include <vector>
#include <string>
using namespace std;

class CWmi
{
public:

    CWmi()
    {
        pclsObj  = NULL;
        pLoc     = NULL;
        pSvc     = NULL;
        pclsEnum = NULL;
        pclsObj  = NULL;

        CoInitializeEx(0, COINIT_MULTITHREADED);
    }

    ~CWmi()
    {
        Release();
        CoUninitialize();
    }


    bool Init(LPCTSTR lpszWMIObj, LPCTSTR lpszWhere=NULL)
    {
        if (!lpszWMIObj || strlen(lpszWMIObj)==0) return false;

        HRESULT hr;

        // Initialize 
        hr = CoInitializeSecurity(

                NULL,     
                -1,      // COM negotiates service                  
                NULL,    // Authentication services
                NULL,    // Reserved
                RPC_C_AUTHN_LEVEL_DEFAULT,    // authentication
                RPC_C_IMP_LEVEL_IMPERSONATE,  // Impersonation
                NULL,             // Authentication info 
                EOAC_NONE,        // Additional capabilities
                NULL              // Reserved
            );

        if (FAILED(hr)) return false;

        
        hr = CoCreateInstance(

                CLSID_WbemLocator,             
                0, 
                CLSCTX_INPROC_SERVER, 
                IID_IWbemLocator, (LPVOID *) &pLoc
            );

        if (FAILED(hr)) return false;


        hr = pLoc->ConnectServer(

                _bstr_t(L"ROOT//CIMV2"), // WMI namespace
                NULL,                    // User name
                NULL,                    // User password
                0,                       // Locale
                NULL,                    // Security flags                 
                0,                       // Authority       
                0,                       // Context object
                &pSvc                    // IWbemServices proxy
            );

        if (FAILED(hr)) return false;

        hr = CoSetProxyBlanket(

                pSvc,                         // the proxy to set
                RPC_C_AUTHN_WINNT,            // authentication service
                RPC_C_AUTHZ_NONE,             // authorization service
                NULL,                         // Server principal name
                RPC_C_AUTHN_LEVEL_CALL,       // authentication level
                RPC_C_IMP_LEVEL_IMPERSONATE,  // impersonation level
                NULL,                         // client identity 
                EOAC_NONE                     // proxy capabilities     
            );

        if (FAILED(hr)) return false;

        hr = pSvc->GetObject(
            bstr_t(lpszWMIObj),
            WBEM_FLAG_RETURN_WBEM_COMPLETE,
            NULL,
            &pclsObj,
            NULL);

        if (FAILED(hr)) return false;


		//构造WQL语句
        bstr_t wql="SELECT * FROM ";
        wql+=lpszWMIObj;

        if (lpszWhere && strlen(lpszWhere))
        {
            wql+=" WHERE ";
            wql+=lpszWhere;
        }

		//查询对象
        hr = pSvc->ExecQuery(
            bstr_t("WQL"), 
            wql,
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY | WBEM_FLAG_RETURN_WBEM_COMPLETE,
            NULL,
            &pclsEnum);

        if (FAILED(hr)) return false;


        //遍历对象，存入Vector数组方便使用
        while (pclsEnum)
        {

            ULONG uReturn = 0;
            IWbemClassObject *pclsObj=NULL;

            hr = pclsEnum->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

            if(0 == uReturn)
            {
                break;
            }

            if (pclsObj) clslist.push_back(pclsObj);
        }


        return true;
    }

	//释放COM对象
    void Release()
    {
        for (vector<IWbemClassObject *>::iterator i=clslist.begin();i!=clslist.end();i++)
        {
            (*i)->Release();
        }

        if (pclsEnum)
        {
            pclsEnum->Release();
            pclsEnum=NULL;
        }

        if (pSvc)
        {
            pSvc->Release();
            pSvc=NULL;
        }

        if (pLoc)
        {
            pLoc->Release();
            pLoc=NULL;
        }

    }

	//返回查询到得总数
    int GetObjCount()
    {
        return clslist.size();
    }

	//获取索引指定对象的的属性
    bstr_t GetProp(int iObj, LPCTSTR lpszPropName )
    {
        bstr_t rstr;

        if ( iObj<clslist.size() )
        {

            _variant_t vtProp;
            //VARIANT vtProp;
            //VariantInit(&vtProp);

            CIMTYPE Type;

            HRESULT hr = clslist[iObj]->Get(bstr_t(lpszPropName), 0, &vtProp, &Type, 0);

            if (SUCCEEDED(hr))
            {
                
                if (vtProp.vt==VT_BSTR)
                {
                    rstr=vtProp.bstrVal;
                }
                else
                if (vtProp.vt & VT_BSTR && vtProp.vt & VT_ARRAY)
                {
                    rstr=SafeArrayToString(vtProp.parray);

                }else{
                    
                    rstr=vtProp;
                }

            }

            VariantClear(&vtProp);

        }

        return rstr;
    }

	//获取对象的方法
    HRESULT GetMethod(LPCTSTR lpszMethodName, IWbemClassObject **ppIn)
    {
        if ( pclsObj )
        {
            return pclsObj->GetMethod(bstr_t(lpszMethodName), 0, ppIn, NULL);
        }

        return SEVERITY_ERROR;
    }

	//SAFEARRAY数组转bstr_t
    static bstr_t SafeArrayToString(SAFEARRAY *psa)
    {
        bstr_t rstr;

        VARIANT* pData=NULL;

        VARTYPE vt;

        SafeArrayGetVartype(psa,&vt);

        int numElements = psa->rgsabound[0].cElements - psa->rgsabound[0].lLbound; 

        HRESULT hr=SafeArrayAccessData(psa,(void HUGEP**)&pData);

        if(SUCCEEDED(hr))
        {
            for(int idx=0; idx < numElements; idx++)
            {
                if (vt==VT_BSTR)
                {
                    rstr += ((BSTR *)(pData))[idx];
                }
            }
            hr = SafeArrayUnaccessData(psa);
        }

        return rstr;
    }

public:


    IWbemLocator  *pLoc;
    IWbemServices *pSvc;

    IEnumWbemClassObject  *pclsEnum;

    IWbemClassObject *pclsObj;
    vector<IWbemClassObject *> clslist;
};

//CWmi的应用：

//设置网关,DNS
void SetGateWays(LPCTSTR lpszGateWays, LPCTSTR lpszDNS1, LPCTSTR lpszDNS2)
{
    CWmi wmi;
    if (wmi.Init("Win32_NetworkAdapterConfiguration", "IPEnabled=TRUE"))
    {
        bstr_t szIndex;

        for (int i=0;i<wmi.GetObjCount();i++)
        {
            szIndex=wmi.GetProp(i,"Index");
            break;
        }

        if (szIndex.length())
        {
            bstr_t szObjPath="Win32_NetworkAdapterConfiguration.Index=";
            szObjPath+=szIndex;

            IWbemClassObject *pSetIn=NULL;
            IWbemClassObject *pSetOut=NULL;

            //Set GateWays
            if (lpszGateWays && strlen(lpszGateWays))
            {
                if ( SUCCEEDED(wmi.GetMethod("SetGateways", &pSetIn)) )
                {


                    SAFEARRAYBOUND Bound[1]={0};
                    Bound[0].lLbound = 0;
                    Bound[0].cElements = 1;

	                //构造参数1
                    bstr_t bsIP=lpszGateWays;
                    BSTR GateWays[1]={bsIP};
                    SAFEARRAY* pArrayGateWays=SafeArrayCreate(VT_BSTR,1,Bound);
                    pArrayGateWays->pvData = GateWays;

                    VARIANT vtDefaultIPGateways;
                    VariantInit(&vtDefaultIPGateways);
                    vtDefaultIPGateways.vt=VT_BSTR | VT_ARRAY;
                    vtDefaultIPGateways.parray=pArrayGateWays;

	                //构造参数2
                    UINT Metrics[1]={1};
                    SAFEARRAY* pArrayMetrics=SafeArrayCreate(VT_I4,1,Bound);
                    pArrayMetrics->pvData = Metrics;

                    VARIANT vtGatewayCostMetrics;
                    VariantInit(&vtGatewayCostMetrics);
                    vtGatewayCostMetrics.vt=(VT_ARRAY|VT_I4);
                    vtGatewayCostMetrics.parray=pArrayMetrics;

                    //设置参数值
			        pSetIn->Put(bstr_t("DefaultIPGateway"),  0, &vtDefaultIPGateways,  0);
			        pSetIn->Put(bstr_t("GatewayCostMetric"), 0, &vtGatewayCostMetrics, 0);

                    //执行SetGateways函数
                    HRESULT hr = wmi.pSvc->ExecMethod(szObjPath, bstr_t("SetGateways"), 0, NULL, pSetIn, &pSetOut, NULL);

                    //释放2个SafeArray参数
                    SafeArrayDestroy(pArrayGateWays);
                    SafeArrayDestroy(pArrayMetrics);

                }
            }

            // SetDNS

            if ( (lpszDNS1 && strlen(lpszDNS1)) || (lpszDNS2 && strlen(lpszDNS2)) )
            {
                if ( SUCCEEDED(wmi.GetMethod("SetDNSServerSearchOrder", &pSetIn)) )
                {


                    SAFEARRAYBOUND Bound[2]={0};
                    Bound[0].lLbound = 0;
                    Bound[0].cElements = 2;

	                //构造参数1

                    bstr_t bsIP1=lpszDNS1;
                    bstr_t bsIP2=lpszDNS2;
                    BSTR DNS[2]={bsIP1, bsIP2};
                    SAFEARRAY* pArrayDNS=SafeArrayCreate(VT_BSTR,1,Bound);
                    pArrayDNS->pvData = DNS;

                    VARIANT vtDNS;
                    VariantInit(&vtDNS);
                    vtDNS.vt=VT_BSTR | VT_ARRAY;
                    vtDNS.parray=pArrayDNS;


                    //设置参数值
			        pSetIn->Put(bstr_t("DNSServerSearchOrder"),  0, &vtDNS,  0);
	
                    //执行SetGateways函数
                    HRESULT hr = wmi.pSvc->ExecMethod(szObjPath, bstr_t("SetDNSServerSearchOrder"), 0, NULL, pSetIn, &pSetOut, NULL);

                    //释放1个SafeArray参数
                    SafeArrayDestroy(pArrayDNS);


                }
            }

        }

    }

}


#endif //_WMI_H_
