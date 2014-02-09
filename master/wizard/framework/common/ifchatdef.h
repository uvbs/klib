
#pragma once

struct IUIMain;

// {D08FC7F5-0853-44DF-9E17-F22B4DC87B16}
static const GUID IID_IChat = 
{ 0xd08fc7f5, 0x853, 0x44df, { 0x9e, 0x17, 0xf2, 0x2b, 0x4d, 0xc8, 0x7b, 0x16 } };


struct IChat
{
    //----------------------------------------------------------------------
    // Summary:
    //      获得用户的个数
    //----------------------------------------------------------------------
    virtual UINT  GetUserCount() = 0;


};

// {88425765-F9BD-4E5D-860E-22E89DA4AAA5}
static const GUID IID_IPlug = 
{ 0x88425765, 0xf9bd, 0x4e5d, { 0x86, 0xe, 0x22, 0xe8, 0x9d, 0xa4, 0xaa, 0xa5 } };

struct IPlug
{
    //virtual 
    virtual CControlUI* CreateUI(CPaintManagerUI*, IUIMain* pUIMain) = 0;
};