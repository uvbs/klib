#pragma once



GUID IID_ITest1Interface = {1,2,3,4};
GUID IID_ITest2Interface = {2,2,3,4};

struct ITest1Interface
{
    virtual HRESULT TestMessageBox() = 0;
};

struct ITest2Interface
{
    virtual HRESULT MyMessageBox() = 0;
};