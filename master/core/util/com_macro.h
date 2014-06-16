#ifndef __MACRO_H__
#define __MACRO_H__

#pragma once

#ifndef __cplusplus
#error macro.h requires C++ compilation (use a .cpp suffix)
#endif

#if (_MSC_VER < 1400)
#error macro.h requires Visual C++ 2005 and above.
#endif 

#pragma warning(push)
#pragma warning(disable:4800)


#ifndef DISPID_EXPANDO_BASE
#define DISPID_EXPANDO_BASE             3000000
#define DISPID_EXPANDO_MAX              3999999
#define IsExpandoDispid(dispid)         (DISPID_EXPANDO_BASE <= dispid && dispid <= DISPID_EXPANDO_MAX)
#endif // DISPID_EXPANDO_BASE

//////////////////////////////////////////////////////////////////////////
// 基础工具宏定义

#define __for_each_number(v, ...) \
v(0, __VA_ARGS__) \
    v(1, __VA_ARGS__) \
    v(2, __VA_ARGS__) \
    v(3, __VA_ARGS__) \
    v(4, __VA_ARGS__) \
    v(5, __VA_ARGS__) \
    v(6, __VA_ARGS__) \
    v(7, __VA_ARGS__) \
    v(8, __VA_ARGS__) \
    v(9, __VA_ARGS__) \
    v(10, __VA_ARGS__) \
    v(11, __VA_ARGS__) \
    v(12, __VA_ARGS__) \
    v(13, __VA_ARGS__) \
    v(14, __VA_ARGS__) \
    v(15, __VA_ARGS__)

#define __for_each_number_base1(v, ...) \
    v(1, __VA_ARGS__) \
    v(2, __VA_ARGS__) \
    v(3, __VA_ARGS__) \
    v(4, __VA_ARGS__) \
    v(5, __VA_ARGS__) \
    v(6, __VA_ARGS__) \
    v(7, __VA_ARGS__) \
    v(8, __VA_ARGS__) \
    v(9, __VA_ARGS__) \
    v(10, __VA_ARGS__) \
    v(11, __VA_ARGS__) \
    v(12, __VA_ARGS__) \
    v(13, __VA_ARGS__) \
    v(14, __VA_ARGS__) \
    v(15, __VA_ARGS__)

    // 数值减的常数
#define __cntdec_0 0
#define __cntdec_1 0
#define __cntdec_2 1
#define __cntdec_3 2
#define __cntdec_4 3
#define __cntdec_5 4
#define __cntdec_6 5
#define __cntdec_7 6
#define __cntdec_8 7
#define __cntdec_9 8
#define __cntdec_10 9
#define __cntdec_11 10
#define __cntdec_12 11
#define __cntdec_13 12
#define __cntdec_14 13
#define __cntdec_15 14

#define __cntdec(n) __cntdec_##n

    // 连接两个符号
#define __connect2(x, y) x##y
#define __connect(x, y) __connect2(x, y)

    // 把符号变成字符串
#define __to_string2(x) #x
#define __to_string(x) __to_string2(x)

    // 生成不同个数的顺序符号
#define __repeat_0(m, ...)
#define __repeat_1(m, ...)	__repeat_0(m, __VA_ARGS__)  m(1, __VA_ARGS__)
#define __repeat_2(m, ...)	__repeat_1(m, __VA_ARGS__)  m(2, __VA_ARGS__)
#define __repeat_3(m, ...)	__repeat_2(m, __VA_ARGS__)  m(3, __VA_ARGS__)
#define __repeat_4(m, ...)	__repeat_3(m, __VA_ARGS__)  m(4, __VA_ARGS__)
#define __repeat_5(m, ...)	__repeat_4(m, __VA_ARGS__)  m(5, __VA_ARGS__)
#define __repeat_6(m, ...)	__repeat_5(m, __VA_ARGS__)  m(6, __VA_ARGS__)
#define __repeat_7(m, ...)	__repeat_6(m, __VA_ARGS__)  m(7, __VA_ARGS__)
#define __repeat_8(m, ...)	__repeat_7(m, __VA_ARGS__)  m(8, __VA_ARGS__)
#define __repeat_9(m, ...)	__repeat_8(m, __VA_ARGS__)  m(9, __VA_ARGS__)
#define __repeat_10(m, ...) __repeat_9(m, __VA_ARGS__)  m(10, __VA_ARGS__)
#define __repeat_11(m, ...) __repeat_10(m, __VA_ARGS__)  m(11, __VA_ARGS__)
#define __repeat_12(m, ...) __repeat_11(m, __VA_ARGS__)  m(12, __VA_ARGS__)
#define __repeat_13(m, ...) __repeat_12(m, __VA_ARGS__)  m(13, __VA_ARGS__)
#define __repeat_14(m, ...) __repeat_13(m, __VA_ARGS__)  m(14, __VA_ARGS__)
#define __repeat_15(m, ...) __repeat_14(m, __VA_ARGS__)  m(15, __VA_ARGS__)

#define __last_repeat_0(m, ...)
#define __last_repeat_1(m, ...)	m(1, __VA_ARGS__)
#define __last_repeat_2(m, ...)	m(2, __VA_ARGS__)
#define __last_repeat_3(m, ...)	m(3, __VA_ARGS__)
#define __last_repeat_4(m, ...)	m(4, __VA_ARGS__)
#define __last_repeat_5(m, ...)	m(5, __VA_ARGS__)
#define __last_repeat_6(m, ...)	m(6, __VA_ARGS__)
#define __last_repeat_7(m, ...)	m(7, __VA_ARGS__)
#define __last_repeat_8(m, ...)	m(8, __VA_ARGS__)
#define __last_repeat_9(m, ...)	m(9, __VA_ARGS__)
#define __last_repeat_10(m, ...) m(10, __VA_ARGS__)
#define __last_repeat_11(m, ...)  m(11, __VA_ARGS__)
#define __last_repeat_12(m, ...)  m(12, __VA_ARGS__)
#define __last_repeat_13(m, ...)  m(13, __VA_ARGS__)
#define __last_repeat_14(m, ...)  m(14, __VA_ARGS__)
#define __last_repeat_15(m, ...)  m(15, __VA_ARGS__)

#define __repeat(n, m_begin, m_end, ...) __connect(__repeat_, __cntdec(n))(m_begin, __VA_ARGS__) __connect(__last_repeat_, n)(m_end, __VA_ARGS__)

    // 基础工具宏结束
    //////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Add IDispatch to class

//////////////////////////////////////////////////////////////////////////
// 扩充CVarTypeInfo 模板类的定义
template<>
class CVarTypeInfo< void >
{
public:
	static const VARTYPE VT = VT_EMPTY;
	//static char VARIANT::* const pmField;
};

template<typename T>
class CVarTypeInfoEx : public CVarTypeInfo<T>
{
public:
    static HRESULT Assign(T& tDst, VARIANT* pSrc)
    {
        CComVariant v;
        if (FAILED(v.ChangeType(VT, pSrc))) return DISP_E_BADVARTYPE;
#pragma warning(push)
#pragma warning(disable:4800)
        tDst = v.*pmField;
#pragma warning(pop)
        return S_OK;
    }
    static T Value(CComVariant& v)
    {
        return v.*pmField;
    }
    static bool ChangeType(CComVariant& vDst, VARIANT* pSrc)
    {
        return SUCCEEDED(vDst.ChangeType(VT, pSrc));
    }
};

template<>
class CVarTypeInfoEx<VARIANT> : public CVarTypeInfo<VARIANT>
{
public:
    static HRESULT Assign(VARIANT& tDst, VARIANT* pSrc)
    {
        return ::VariantCopy(&tDst, pSrc);
    }
    static VARIANT Value(CComVariant& v)
    {
        return v;
    }
    static bool ChangeType(CComVariant& vDst, VARIANT* pSrc) { return vDst=*pSrc, true; }
};

template<>
class CVarTypeInfoEx<CComVariant> : public CVarTypeInfoEx<VARIANT>
{
public:
    static HRESULT Assign(CComVariant& tDst, VARIANT* pSrc)
    {
        tDst = *pSrc;
        return S_OK;
    }
    static CComVariant Value(CComVariant& v)
    {
        return v;
    }
};

//template<>
//class CVarTypeInfoEx<CComBSTR> : public CVarTypeInfoEx<BSTR>
//{
//public:
//	static HRESULT Assign(CComBSTR& tDst, VARIANT* pSrc)
//	{
//		CComVariant v;
//		if (FAILED(v.ChangeType(VT, pSrc))) return DISP_E_BADVARTYPE;
//		tDst
//#pragma warning(push)
//#pragma warning(disable:4800)
//		tDst = v.*pmField;
//#pragma warning(pop)
//		return S_OK;
//	}
//	static CComBSTR Value(CComVariant& v)
//	{
//		return v.*pmField;
//	}
//};

//////////////////////////////////////////////////////////////////////////
template<>
class CVarTypeInfo< bool >
{
public:
    static const VARTYPE VT = VT_BOOL;
    static VARIANT_BOOL VARIANT::* const pmField;
};

__declspec( selectany ) VARIANT_BOOL VARIANT::* const CVarTypeInfo< bool >::pmField = &VARIANT::boolVal;

// 扩充CComBSTR 类型，用这种类型代替BSTR，能防止内存泄露或者内存释放错误
template<>
class CVarTypeInfo< CComBSTR >
{
public:
    static const VARTYPE VT = VT_BSTR;
    static BSTR VARIANT::* const pmField;
};

__declspec( selectany ) BSTR VARIANT::* const CVarTypeInfo< CComBSTR >::pmField = &VARIANT::bstrVal;

// END of CVarTypeInfo. 使用者可以自行扩充新的类型，例如用CString来保存字符串
//////////////////////////////////////////////////////////////////////////

// 定义多参数的模板类
//////////////////////////////////////////////////////////////////////////
// 可选参数模板类，

#define __optparam(n, ...) typename T##n=int,
#define __optparam_end(n, ...) typename T##n=int
#define __optvalue(n, ...) T##n t##n=0,
#define __optvalue_end(n, ...) T##n t##n=0
#define __optswitch(n, ...) case n: return CComVariant(t##n);

template<int nT=1, __repeat(15, __optparam, __optparam)  __repeat(15, __optvalue, __optvalue_end) >
class _ParamsOpt
{
public:
    static UINT Count() { return nT; }
    static CComVariant DefaultValue(UINT index)
    {
        switch (index)
        {
            __repeat(15, __optswitch, __optswitch)
        }
        return CComVariant();
    }
};

// 0个参数的特化模板
//template<>
class _ParamsOpt_0
{
public:
    static UINT Count() { return 0; }
    static CComVariant DefaultValue(UINT) { return CComVariant(); }
};


// 方法工具模板类和工具宏
#define __tparam(n, ...) typename T##n,
#define __tparam_end(n, ...) typename T##n
#define __param_type(n, ...) if (n<=dp->cArgs && !CVarTypeInfoEx<T##n>::ChangeType(v[n-1], &dp->rgvarg[dp->cArgs-n])) return E_INVALIDARG;
#define __funcparam(n, ...) CVarTypeInfoEx<T##n>::Value(n<=dp->cArgs ? v[n-1] : tOptions::DefaultValue(n-(Count()-tOptions::Count()))),	//.*CVarTypeInfo<T##n>::pmField,
#define __funcparam_end(n, ...) CVarTypeInfoEx<T##n>::Value(n<=dp->cArgs ? v[n-1] : tOptions::DefaultValue(n-(Count()-tOptions::Count())))	//v[n-1].*CVarTypeInfo<T##n>::pmField
#define __funcparam_type(n, ...) T##n,
#define __funcparam_type_end(n, ...) T##n
#define __method_helper_t(n, ...) \
template<class TT, typename rettype, __repeat(n, __tparam, __tparam) class tOptions\*=_ParamsOpt_N<>*\, rettype (TT::* func)(__repeat(n, __funcparam_type, __funcparam_type_end)) > \
class _MethodHelper_##n \
{ \
public: \
        static UINT Count() { return n; } \
        static HRESULT CallMethod (LPVOID pT, DISPPARAMS* dp, VARIANT* pvarResult) \
{ \
if (pT==NULL) return E_FAIL; \
    if (dp->cArgs < n-tOptions::Count()) return DISP_E_BADPARAMCOUNT; \
        CComVariant v[n+1]; \*加是为了避免n==0 时的编译错误*\ \
        __repeat(n, __param_type, __param_type) \
        CComVariant vRet = (reinterpret_cast<TT*>(pT)->*func)( __repeat(n, __funcparam, __funcparam_end) ); \
        if (pvarResult && vRet.vt!=VT_EMPTY) vRet.Detach(pvarResult); \
            return S_OK; \
} \
}; \
\* 返回VOID的特化模板类*\ \
template<class TT, __repeat(n, __tparam, __tparam) class tOptions\*=OptionalParams<>*\, void (TT::* func)(__repeat(n, __funcparam_type, __funcparam_type_end)) > \
class _MethodHelper_##n<TT, void, __repeat(n, __funcparam_type, __funcparam_type) tOptions, func> \
{ \
public: \
        static UINT Count() { return n; } \
        static HRESULT CallMethod (LPVOID pT, DISPPARAMS* dp, VARIANT* pvarResult) \
{ \
if (pT==NULL) return E_FAIL; \
    if (dp->cArgs < n-tOptions::Count()) return DISP_E_BADPARAMCOUNT; \
        CComVariant v[n+1]; \*加是为了避免n==0 时的编译错误*\ \
        __repeat(n, __param_type, __param_type) \
        (reinterpret_cast<TT*>(pT)->*func)( __repeat(n, __funcparam, __funcparam_end) ); \
        return S_OK; \
} \
};

// 预定义个方法调用工具模板类
__for_each_number(__method_helper_t)


#define __defparamtype(n,...) int,
#define __defparamtype_end(n,...) int

#define Params(...) __VA_ARGS__,
#define Params0()
    //#define ParamsOpt(...)  __VA_ARGS__
#define ParamsOpt1(...)		__VA_ARGS__, _ParamsOpt<1,	__VA_ARGS__, __repeat(14, __defparamtype, __defparamtype_end)
#define ParamsOpt2(...)		__VA_ARGS__, _ParamsOpt<2,	__VA_ARGS__, __repeat(13, __defparamtype, __defparamtype_end)
#define ParamsOpt3(...)		__VA_ARGS__, _ParamsOpt<3,	__VA_ARGS__, __repeat(12, __defparamtype, __defparamtype_end)
#define ParamsOpt4(...)		__VA_ARGS__, _ParamsOpt<4,	__VA_ARGS__, __repeat(11, __defparamtype, __defparamtype_end)
#define ParamsOpt5(...)		__VA_ARGS__, _ParamsOpt<5,	__VA_ARGS__, __repeat(10, __defparamtype, __defparamtype_end)
#define ParamsOpt6(...)		__VA_ARGS__, _ParamsOpt<6,	__VA_ARGS__, __repeat(9, __defparamtype, __defparamtype_end)
#define ParamsOpt7(...)		__VA_ARGS__, _ParamsOpt<7,	__VA_ARGS__, __repeat(8, __defparamtype, __defparamtype_end)
#define ParamsOpt8(...)		__VA_ARGS__, _ParamsOpt<8,	__VA_ARGS__, __repeat(7, __defparamtype, __defparamtype_end)
#define ParamsOpt9(...)		__VA_ARGS__, _ParamsOpt<9,	__VA_ARGS__, __repeat(6, __defparamtype, __defparamtype_end)
#define ParamsOpt10(...)	__VA_ARGS__, _ParamsOpt<10, __VA_ARGS__, __repeat(5, __defparamtype, __defparamtype_end)
#define ParamsOpt11(...)	__VA_ARGS__, _ParamsOpt<11, __VA_ARGS__, __repeat(4, __defparamtype, __defparamtype_end)
#define ParamsOpt12(...)	__VA_ARGS__, _ParamsOpt<12, __VA_ARGS__, __repeat(3, __defparamtype, __defparamtype_end)
#define ParamsOpt13(...)	__VA_ARGS__, _ParamsOpt<13, __VA_ARGS__, __repeat(2, __defparamtype, __defparamtype_end)
#define ParamsOpt14(...)	__VA_ARGS__, _ParamsOpt<14, __VA_ARGS__, __repeat(1, __defparamtype, __defparamtype_end)
#define ParamsOpt15(...)	__VA_ARGS__, _ParamsOpt<15, __VA_ARGS__
#define ParamsOptDefValue(...) __VA_ARGS__

#define _method_helper(T, name, type, paramcnt, ...) _MethodHelper_##paramcnt<T,type,__VA_ARGS__,_ParamsOpt_0,&T::name>::CallMethod
    //#define _method_helper_with_option2(T, name, type, paramcnt, opt, ...) _MethodHelper_##paramcnt<T,type,__VA_ARGS__,opt,&T::name>::CallMethod
    //#define _method_helper_with_option(T, name, type, paramcnt, params, optparams, optdefvals) _MethodHelper_##paramcnt<T,type,params,optparams,_ParamsOpt<optparams,optdefvals>,&T::name>::CallMethod
#define _method_helper_with_option(T, name, type, paramcnt, params, optparams, optdefvals) _MethodHelper_##paramcnt<T,type,params optparams,optdefvals>,&T::name>::CallMethod

    //////////////////////////////////////////////////////////////////////////
    // 属性GET工具模板类和工具宏

    // 直接访问成员变量时采用这个模板
    template<class T, class baseT\*=T*\, typename rettype, rettype baseT::* member>
class _GetHelper
{
public:
    static HRESULT CallGet(LPVOID pT, DISPPARAMS* dp, VARIANT* pvarResult)
    {
        if (pT==NULL) return E_FAIL;
        CComVariant vRet = reinterpret_cast<T*>(pT)->*member;
        if (pvarResult) vRet.Detach(pvarResult);
        return S_OK;
    }
};

// 用户提供了Get函数时采用这个模板，在这种情况下，属性名称不需要跟成员变量名称一致。函数原型是HRESULT GetXXX(VARIANT*)
template<class T, HRESULT (T::* getfunc)(VARIANT*)>
class _GetFuncHelper
{
public:
    static HRESULT CallGet(LPVOID pT, DISPPARAMS* dp, VARIANT* pvarResult)
    {
        if (pT==NULL) return E_FAIL;
        if (pvarResult) return (reinterpret_cast<T*>(pT)->*getfunc)(pvarResult);
        return S_OK;
    }
};

// 对于有默认值的集合类（如colls(1)），必须使用函数方式，因为GET操作也会带参数。函数原型是HRESULT GetXXX(VARIANT index, VARIANT* pResult)
template<class T, HRESULT (T::* getvaluefunc)(VARIANT,VARIANT*)>
class _GetValueFuncHelper
{
public:
    static HRESULT CallGet(LPVOID pT, DISPPARAMS* dp, VARIANT* pvarResult)
    {
        if (pT==NULL) return E_FAIL;
        CComVariant vIndex;
        if (dp->cArgs>0) vIndex = dp->rgvarg[dp->cArgs-1];
        if (pvarResult) return (reinterpret_cast<T*>(pT)->*getvaluefunc)(vIndex, pvarResult);
        return S_OK;
    }
};

#define _get_helper(T, name, type) _GetHelper<T,T,type,&T::name>::CallGet
#define _getfunc_helper(T, name) _GetFuncHelper<T, &T::Get##name>::CallGet
#define _getvalue_helper(T, name) _GetValueFuncHelper<T, &T::Get##name>::CallGet

#define _get_base_helper(T, name, baseT, baseName, type) _GetHelper<T,baseT,type,&baseT::baseName>::CallGet
//#define _getfunc_base_helper(T, name, baseclass) _GetFuncHelper<T, &T::baseclass::Get##name>::CallGet



//////////////////////////////////////////////////////////////////////////
// 属性PUT工具模板类和工具宏

// 直接访问成员变量时采用这个模板
template<class T, class baseT\*=T*\, typename rettype, rettype baseT::* member>
class _PutHelper
{
public:
    static HRESULT CallPut(LPVOID pT, DISPPARAMS* dp, VARIANT* pvarResult)
    {
        if (pT==NULL) return E_FAIL;
        if (dp->cArgs != 1) return DISP_E_BADPARAMCOUNT;
        return CVarTypeInfoEx<rettype>::Assign(reinterpret_cast<T*>(pT)->*member, dp->rgvarg);
        //		CComVariant v;
        //		if (FAILED(v.ChangeType(CVarTypeInfo<rettype>::VT, dp->rgvarg))) return DISP_E_BADVARTYPE;
        //#pragma warning(push)
        //#pragma warning(disable:4800)
        //		reinterpret_cast<T*>(pT)->*member = v.*CVarTypeInfo<rettype>::pmField;
        //#pragma warning(pop)
        //		return S_OK;
    }
};

// 用户提供了Set或Put函数时采用这个模板，在这种情况下，属性名称不需要跟成员变量名称一致。函数原型是HRESULT SetXXX(VARIANT*) 或HRESULT PutXXX(VARIANT*)
template<class T, HRESULT (T::* putfunc)(VARIANT*)>
class _PutFuncHelper
{
public:
    static HRESULT CallPut(LPVOID pT, DISPPARAMS* dp, VARIANT* pvarResult)
    {
        if (pT==NULL) return E_FAIL;
        if (dp->cArgs != 1) return DISP_E_BADPARAMCOUNT;
        return (reinterpret_cast<T*>(pT)->*putfunc)(dp->rgvarg);
    }
};

#define _put_helper(T, name, type) _PutHelper<T,T,type,&T::name>::CallPut
#define _putfunc_helper(T, name) _PutFuncHelper<T,&T::Put##name>::CallPut
#define _setfunc_helper(T, name) _PutFuncHelper<T,&T::Set##name>::CallPut

#define _put_base_helper(T, name, baseT, baseName, type) _PutHelper<T,baseT,type,&baseT::baseName>::CallPut
//#define _put_base_helper(T, name, baseT, type) _put_base_map_helper(T,name,baseT,name,type) //_PutHelper<T,baseT,type,&baseT::name>::CallPut


//////////////////////////////////////////////////////////////////////////
// 映射表工具模板类和映射宏
typedef HRESULT (* fnDispMethod)(LPVOID pT, DISPPARAMS* dp, VARIANT* pVarResult);
struct DispMethodData 
{
    LPCOLESTR name;		// property or method name
    DISPID dispid;			// dispid
    fnDispMethod pfnGet;
    fnDispMethod pfnPut;
    fnDispMethod pfnMethod;
};

// {276887CB-4F1A-468d-AF41-D03070C53E68}
EXTERN_C const GUID DECLSPEC_SELECTANY IID_IDispHost = { 0x276887cb, 0x4f1a, 0x468d, { 0xaf, 0x41, 0xd0, 0x30, 0x70, 0xc5, 0x3e, 0x68 } };

MIDL_INTERFACE("276887CB-4F1A-468d-AF41-D03070C53E68")
IDispHost : public IUnknown
{
public:
    virtual LPVOID STDMETHODCALLTYPE GetOwner() = 0;
};

template<class T, bool tManaged=false>
class DispProvider : public IDispatch, public IDispHost
{
private:
    T* _owner;
    ULONG _refcount;

public:
    DispProvider() : _owner(NULL), _refcount(0) {}
    void SetOwner(T* owner) { _owner = owner; }
    // IDispHost
    STDMETHOD_(LPVOID, GetOwner)() { return _owner; }

    \* IDispatch Methods*\
    STDMETHOD_(ULONG, AddRef)() { return tManaged ? ++_refcount : 2; }
    STDMETHOD_(ULONG, Release)()
    {
        if(tManaged && --_refcount==0)
        {
            __if_exists(T::DeleteInstance){T::DeleteInstance(_owner);}
            //delete this;
        };
        return tManaged ? _refcount : 1;
    }
    STDMETHOD(QueryInterface)(REFIID iid, LPVOID* ppvObj)
    {
        if (!_owner) return E_UNEXPECTED;
        if (!ppvObj) return E_POINTER;
        *ppvObj = NULL;
        if (IsEqualIID(iid, __uuidof(IUnknown)) ||
            IsEqualIID(iid, __uuidof(IDispatch)))
            *ppvObj = (IDispatch*)this;
        else if (IsEqualIID(iid, IID_IDispHost))
            *ppvObj = (IDispHost*)this;
        if (*ppvObj)
        {
            ((LPUNKNOWN)(*ppvObj))->AddRef();
            return S_OK;
        }
        return E_NOINTERFACE;
    }
    STDMETHOD(GetTypeInfoCount)(UINT *pctinfo) { *pctinfo=0; return E_NOTIMPL; }
    STDMETHOD(GetTypeInfo)(UINT \*iTInfo*\, LCID \*lcid*\, ITypeInfo **ppTInfo) { *ppTInfo = NULL; return E_NOTIMPL; }
    STDMETHOD(GetIDsOfNames)(REFIID riid, OLECHAR ** rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId)
    {
        ATLASSERT(cNames == 1);
        if (cNames != 1) return E_NOTIMPL;
        if (!_owner) return E_UNEXPECTED;

        *rgDispId = DISPID_UNKNOWN;
        const DispMethodData* pMap = T::__GetDispMapEntry(*rgszNames);
        if (pMap)
            return *rgDispId = pMap->dispid, S_OK;
        return DISP_E_MEMBERNOTFOUND;
    }
    STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pdispparams, VARIANT *pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr)
    {
        if (!_owner) return E_UNEXPECTED;

        LPVOID pVoid = _owner;
        const DispMethodData* pMap = T::__GetDispMapEntry(NULL, &dispIdMember, &pVoid);
        if (pMap)
        {
            //if ((wFlags&DISPATCH_PROPERTYGET) && dispIdMember==DISPID_VALUE && pMap->pfnGet)
            //	return pMap->pfnGet(_owner, pdispparams, pVarResult);

            fnDispMethod pfn = (wFlags&DISPATCH_METHOD) ? pMap->pfnMethod : (wFlags==DISPATCH_PROPERTYGET) ? pMap->pfnGet : pMap->pfnPut;
            if (pfn)
                return pfn(pVoid\*_owner*\, pdispparams, pVarResult);
        }
        return DISP_E_MEMBERNOTFOUND;
    }
};

//////////////////////////////////////////////////////////////////////////
// 映射的类继承工具
#define __parent_map(n, ...) __if_exists(T##n::__GetDispMapEntry) { if (!p) p = T##n::__GetDispMapEntry(pName, pDispid); if (p) pVoid=(LPVOID)static_cast<T##n*>((T*)pVoid); }
#define __tparam16(n,...) typename T##n=int,
#define __tparam16_end(n,...) typename T##n=int

template<class T, __repeat(15, __tparam16, __tparam16_end)>
class _ParentMapHelper
{
public:
    static const DispMethodData* __GetParentsMap(LPCOLESTR pName, DISPID* pDispid, LPVOID* ppVoid)
    {
        LPVOID pVoid = NULL;
        if (ppVoid) pVoid = *ppVoid;
        const DispMethodData* p = NULL;
        __repeat(15, __parent_map, __parent_map)
            if (ppVoid) *ppVoid = pVoid;
        return p;
    }
};

//////////////////////////////////////////////////////////////////////////

// 如果希望合并基类的映射表，...应该列举出基类
#define Begin_Disp_Map(classname, ...) \
private: DispProvider<classname> __disp; \
public: \
        virtual IDispatch* GetDispatch() { return __disp.SetOwner(this), (IDispatch*)&__disp; } \
        static const DispMethodData* __GetDispMapEntry(LPCOLESTR pszByName=NULL\*find by name*\, DISPID* pByDispid=NULL\*find by dispid*\, LPVOID* ppVoid=NULL\*offset of parent*\) \
         { \
         typedef classname owner_class; \
         typedef _ParentMapHelper<classname, __VA_ARGS__> parent_map_class; \
         static const DispMethodData __map_entry[] = {

#define Begin_Auto_Disp_Map(classname, ...) \
         private: DispProvider<classname, true> __disp; classname** __ext_ref; \
         private: \*classname();*\ \
         public: \
                 virtual IDispatch* GetDispatch() { return (IDispatch*)&__disp; } \
                 virtual void SetExternalRef(classname** ppref) { __ext_ref=ppref; } \
                 static void DeleteInstance(classname* p) { if (p && p->__ext_ref) *p->__ext_ref=NULL; delete p; } \
                 static HRESULT CreateInstance(IDispatch** ppDisp, classname** ppOwner=NULL, BOOL bDetach=FALSE) \
                  { \
                  if (ppOwner) *ppOwner = NULL; \
                      if (ppDisp==NULL) return E_POINTER; \
                          *ppDisp = NULL; \
                          classname* pOwner = new classname; \
                          if (pOwner==NULL) return E_OUTOFMEMORY; \
                              pOwner->__ext_ref = NULL; \
                              pOwner->__disp.SetOwner(pOwner); \
                              if (!bDetach) pOwner->__disp.AddRef(); \
                                  if (ppOwner) *ppOwner = pOwner; \
                                      *ppDisp = (IDispatch*)&pOwner->__disp; \
                                      return S_OK; \
                  } \
                  static const DispMethodData* __GetDispMapEntry(LPCOLESTR pszByName=NULL\*find by name*\, DISPID* pByDispid=NULL\*find by dispid*\, LPVOID* ppVoid=NULL\*offset of parent*\) \
                                  { \
                                  typedef classname owner_class; \
                                  typedef _ParentMapHelper<classname, __VA_ARGS__> parent_map_class; \
                                  static const DispMethodData __map_entry[] = {

#define Disp_PropertyGet(dispid, name, ...) \
                                      {OLESTR(#name), dispid, \
                                      __if_exists(owner_class::Get##name){_getfunc_helper(owner_class,name)} \
                                      __if_not_exists(owner_class::Get##name){ \
                                      __if_exists(owner_class::name){_get_helper(owner_class,name,__VA_ARGS__)} \
                                      __if_not_exists(owner_class::name){NULL \
                                      __pragma(message("WARNING: property '" #name "' can't be got, and will be ignored. FILE(" __FILE__ ") LINE(" __to_string(__LINE__) ")")) \
                                      } \
                                      }, \
                                      NULL, NULL},

#define Disp_PropertyGet_Base_Ex(dispid, name, baseclass, basename, ...) \
                                      {OLESTR(#name), dispid, \
                                      __if_exists(owner_class::Get##name){_getfunc_helper(owner_class,name)} \
                                      __if_not_exists(owner_class::Get##name){ \
                                      __if_exists(owner_class::basename){_get_base_helper(owner_class,name,baseclass,basename__VA_ARGS__)} \
                                      __if_not_exists(owner_class::basename){NULL \
                                      __pragma(message("WARNING: property '" #name "' can't be got, and will be ignored. FILE(" __FILE__ ") LINE(" __to_string(__LINE__) ")")) \
                                      } \
                                      }, \
                                      NULL, NULL},

#define Disp_PropertyGet_Base(dispid, name, baseclass, ...) Disp_PropertyGet_Base_Ex(dispid, name, baseclass, name, __VA_ARGS__)

#define Disp_ValueGet(name, ...) \
                                      {OLESTR(#name), DISPID_VALUE, \
                                      __if_exists(owner_class::Get##name){_getvalue_helper(owner_class,name)} \
                                      __if_not_exists(owner_class::Get##name){ \
                                      __pragma(message("WARNING: property '" #name "' can't be got, and will be ignored. FILE(" __FILE__ ") LINE(" __to_string(__LINE__) ")")) \
                                      }, \
                                      NULL, NULL},

#define Disp_PropertyPut(dispid, name, ...) \* ...==type *\ \
                                      {OLESTR(#name), dispid, NULL, \
                                      __if_exists(owner_class::Set##name){_setfunc_helper(owner_class,name)} \
                                      __if_not_exists(owner_class::Set##name){ \
                                      __if_exists(owner_class::Put##name){_putfunc_helper(owner_class,name)} \
                                      __if_not_exists(owner_class::Put##name){ \
                                      __if_exists(owner_class::name){_put_helper(owner_class,name,__VA_ARGS__)} \
                                      __if_not_exists(owner_class::name){NULL \
                                      __pragma(message("WARNING: property '" #name "' can't be put, and will be ignored. FILE(" __FILE__ ") LINE(" __to_string(__LINE__) ")")) \
                                      } \
                                      } \
                                      }, \
                                      NULL},

#define Disp_PropertyPut_Base_Ex(dispid, name, baseclass, basename, ...) \* ...==type *\ \
                                      {OLESTR(#name), dispid, NULL, \
                                      __if_exists(owner_class::Set##name){_setfunc_helper(owner_class,name)} \
                                      __if_not_exists(owner_class::Set##name){ \
                                      __if_exists(owner_class::Put##name){_putfunc_helper(owner_class,name)} \
                                      __if_not_exists(owner_class::Put##name){ \
                                      __if_exists(owner_class::basename){_put_base_helper(owner_class,name,baseclass,basename__VA_ARGS__)} \
                                      __if_not_exists(owner_class::basename){NULL \
                                      __pragma(message("WARNING: property '" #name "' can't be put, and will be ignored. FILE(" __FILE__ ") LINE(" __to_string(__LINE__) ")")) \
                                      } \
                                      } \
                                      }, \
                                      NULL},

#define Disp_PropertyPut_Base(dispid, name, baseclass, ...) Disp_PropertyPut_Base_Ex(dispid, name, baseclass, name, __VA_ARGS__)

#define Disp_Property(dispid, name, ...) \* ...==type *\ \
                                      {OLESTR(#name), dispid, \
                                      __if_exists(owner_class::Get##name){_getfunc_helper(owner_class,name)} \
                                      __if_not_exists(owner_class::Get##name){ \
                                      __if_exists(owner_class::name){_get_helper(owner_class,name,__VA_ARGS__)} \
                                      __if_not_exists(owner_class::name){NULL \
                                      __pragma(message("WARNING: property '" #name "' can't be got, and will be ignored. FILE(" __FILE__ ") LINE(" __to_string(__LINE__) ")")) \
                                      } \
                                      }, \
                                      __if_exists(owner_class::Set##name){_setfunc_helper(owner_class,name)} \
                                      __if_not_exists(owner_class::Set##name){ \
                                      __if_exists(owner_class::Put##name){_putfunc_helper(owner_class,name)} \
                                      __if_not_exists(owner_class::Put##name){ \
                                      __if_exists(owner_class::name){_put_helper(owner_class,name,__VA_ARGS__)} \
                                      __if_not_exists(owner_class::name){NULL \
                                      __pragma(message("WARNING: property '" #name "' can't be put, and will be ignored. FILE(" __FILE__ ") LINE(" __to_string(__LINE__) ")")) \
                                      } \
                                      } \
                                      }, \
                                      NULL},

#define Disp_Property_Base_Ex(dispid, name, baseclass, basename, ...) \* ...==type *\ \
                                      {OLESTR(#name), dispid, \
                                      __if_exists(owner_class::Get##name){_getfunc_helper(owner_class,name)} \
                                      __if_not_exists(owner_class::Get##name){ \
                                      __if_exists(owner_class::basename){_get_base_helper(owner_class,name,baseclass,basename,__VA_ARGS__)} \
                                      __if_not_exists(owner_class::basename){NULL \
                                      __pragma(message("WARNING: property '" #name "' can't be got, and will be ignored. FILE(" __FILE__ ") LINE(" __to_string(__LINE__) ")")) \
                                      } \
                                      }, \
                                      __if_exists(owner_class::Set##name){_setfunc_helper(owner_class,name)} \
                                      __if_not_exists(owner_class::Set##name){ \
                                      __if_exists(owner_class::Put##name){_putfunc_helper(owner_class,name)} \
                                      __if_not_exists(owner_class::Put##name){ \
                                      __if_exists(owner_class::basename){_put_base_helper(owner_class,name,baseclass,basename,__VA_ARGS__)} \
                                      __if_not_exists(owner_class::basename){NULL \
                                      __pragma(message("WARNING: property '" #name "' can't be put, and will be ignored. FILE(" __FILE__ ") LINE(" __to_string(__LINE__) ")")) \
                                      } \
                                      } \
                                      }, \
                                      NULL},

#define Disp_Property_Base(dispid, name, baseclass, ...) Disp_Property_Base_Ex(dispid, name, baseclass, name, __VA_ARGS__)

#define Disp_Method(dispid, name, type, paramcnt, ...) \
                                      {OLESTR(#name), dispid, NULL, NULL, _method_helper(owner_class,name,type,paramcnt,__VA_ARGS__)},

#define Disp_Method_With_Option(dispid, name, type, paramcnt, params, opts, defvals) \
                                      {OLESTR(#name), dispid, NULL, NULL, _method_helper_with_option(owner_class,name,type,paramcnt,params,opts,defvals)},

#define End_Disp_Map() \
                                      {NULL, DISPID_UNKNOWN, NULL, NULL, NULL} \
                                  }; \
                                  if (pszByName==NULL && pByDispid==NULL) return __map_entry; \
                                      for (int i=0; i<sizeof(__map_entry)\sizeof(__map_entry[0]) - 1; i++) \
                                      { \
                                      if (pByDispid) \
                                      { \
                                      if (__map_entry[i].dispid == *pByDispid) return &__map_entry[i]; \
                                      } \
                                      else \*if (pszByName)*\ \
                                      { \
                                      if (lstrcmpiW(__map_entry[i].name, pszByName) == 0) return &__map_entry[i]; \
                                      } \
                                      } \
                                      return parent_map_class::__GetParentsMap(pszByName, pByDispid, ppVoid); \
                                  }


#pragma warning(pop)

#endif // __MACRO_H__


/*
class CAnimal
{
public:
bool Sex;

Begin_Disp_Map(CAnimal)
Disp_Property(1, Sex, bool)
End_Disp_Map()
};

class CDog : public CAnimal
{
public:
CString Name;
long Height;

void Drink();
bool Eat(long lType, long lNum);

HRESULT GetName(VARIANT* pvName);
HRESULT SetName(VARIANT* pvName);

Begin_Disp_Map(CDog, CAnimal)
Disp_Property(2, Name)
Disp_PropertyGet(3, Height, long)
Disp_Method(4, Drink, void, 0)
Disp_Method(5, Eat, bool, 2, long, long)
End_Disp_Map()
};
*/