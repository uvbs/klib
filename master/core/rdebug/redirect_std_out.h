
#pragma once

#include <iostream>
#include <algorithm>
#include <sstream>
#include <tchar.h>

namespace klib{
namespace debug{

using namespace std;
template<class _Elem, class _Traits = char_traits<_Elem> >
class CDebugStreamBuf : public basic_stringbuf<_Elem, _Traits >
{    // 回调函数接受一个准备输出到设备的字符串参数。
public:    // 如果回调函数返回false，缓存对象的_Mystate将被设为BAD，并不再继续写入。
    typedef bool (__stdcall *_Myof)( const _Elem* );
    explicit inline CDebugStreamBuf( _Myof _Outfunc )
        : _Myoutfunc( _Outfunc ){}    // 初始化回调函数
    virtual ~CDebugStreamBuf( void ){}    // 虚析构函数
protected:    // 每次同步都会刷新缓存中的所有内容
    virtual int sync( void )    // 重载sync函数，将在需要输入到设备（同步）时被调用
    {    // 如果无回调函数或缓冲未初始化，返回错误。_Mystate将被设为BAD
        if ( _Myoutfunc == 0 || pptr() == 0 ) return -1;
        else if ( _Mysb::pbase() >= _Mysb::epptr() ) return 0;
        else    // 确保缓存可以写入
        {    // 将当前写指针的后一位改写为0，如果缓存长度不足将调用overflow执行添加
            sputc( _Traits::to_char_type( 0 ) );
            setg( pbase(), pbase(), pptr() );    // 将读指针设为缓存起点
            seekpos( 0, ios_base::out );    // 将写指针置0以备下次从起点写入
            for ( ; gptr() < egptr(); )    // 遍例
            {    //忽略包括当前指针在内后面的所有0值
                for ( ; !sbumpc(); );
                if ( !_Myoutfunc( gptr() - 1 ) ) return -1;    // 输出字符串
                gbump( (int)( find( gptr(), egptr(),    // 找到下一个0值
                    _Traits::to_char_type( 0 ) ) - gptr() + 1 ) );
            }
        }
        return 0;
    }
protected:
    _Myof _Myoutfunc;    // 回调函数成员，必须在构造时初始化
};




}}
