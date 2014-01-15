#ifndef _klib_proto_dispatcher_j_
#define _klib_proto_dispatcher_j_


#include <map>
#include <functional>

using namespace std::tr1::placeholders;

namespace klib{
namespace net {
    

template<typename cmd_type, typename func_type>
class dispatcher_mgr
{
public:
    typedef dispatcher_mgr<cmd_type, func_type> self_type;

public:
    bool register_dispatcher(cmd_type cmd, const func_type& func)
    {
        auto itr = dispatchers_.find(cmd);
        if (itr == dispatchers_.end()) {
            dispatchers_.insert(std::make_pair(cmd, func));
            return true;
        }
        return false;
    }

    bool find_dispatcher(cmd_type cmd, func_type& f)
    {
        auto itr = dispatchers_.find(cmd);
        if (itr == dispatchers_.end()) {
            return false;
        }
        f = itr->second;
        return true;
    }

    void clear()
    {
        dispatchers_.clear();
    }

protected:
    ::std::map<cmd_type, func_type>  dispatchers_;
};


//////////////////////////////////////////////////////////////////////////
//开始注册处理函数
#define BEGIN_REGISTER_DIAPATCH(dispatcher_)		\
	void register_dispatcher_()			\
	{

//根据命令注册处理函数
#define		REGISTER_DISPATCH(cmd, Function)			\
	dispatcher_.register_dispatcher(cmd, Function);

//结束注册处理函数
#define END_REGISTER_DISPATCH		\
	}



/*


//使用示例
typedef std::function<void(DWORD, USHORT, cmd_header*, net_archive*, BOOL&)> fdDispatchFunction;
dispatcher_mgr<UINT, fdDispatchFunction> dispatcher_;

BEGIN_REGISTER_DIAPATCH(dispatcher_)
    RREGISTER_DISPATCH(1, std::bind(&Class::MemberFunc, this, _1, ...))
END_REGISTER_DISPATCH

*/

}}




#endif // _klib_proto_dispatcher_j_