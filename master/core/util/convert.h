#ifndef _klib_convert_h_
#define _klib_convert_h_

#include <string>
#include <codecvt>
#include <sstream>

namespace klib {
namespace util {

namespace converter{


template<class T>
std::string to_string(const T& t)
{
    ostringstream oss;
    oss << t;
    return std::string(oss.str());
}

template<class out_type, class in_type>
out_type convert(const in_type & t)
{
	stringstream stream;
	out_type result;	//这里存储转换结果

	stream << t;		//向流中传值
	stream >> result;	//向result中写入值

	return result;
}



}

}}

#endif // _klib_convert_h_