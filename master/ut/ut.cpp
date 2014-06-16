// ut.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


#ifdef _DEBUG
#pragma comment(lib, "gtest_d.lib")
#pragma comment(lib, "gtest_main_d.lib")
#else
#pragma comment(lib, "gtest.lib")
#pragma comment(lib, "gtest_main.lib")
#endif

//#pragma comment(lib, "ws2_32")

#include <core/atom.h>
#include <klib_link.h>

#include "find_tool_test.h"
#include "faststring_test.h"
#include "object_resuer_test.h"
#include "mem_string_test.h"
#include "nocopyable_test.h"
#include "mem_buffer_test.h"
#include "private_heap_test.h"
#include "mem_list_test.h"
#include "array_template_test.h"
#include "strutil_test.h"
#include "queue_test.h"
#include "aspect_test.h"
#include "trie_test.h"
#include "factory_test.h"
#include "qsort_test.h"
#include "sync_invoke_test.h"
#include "kmp_test.h"
#include "bitmap_test.h"
#include "bloom_test.h"
#include "stream_test.h"
#include "actor_test.h"
#include "active_object_test.h"
#include "cat_str_macro_test.h"
#include "must_have_base_test.h"
#include "redirect_std_out_test.h"
#include "ensure_test.h"
#include "multi_int_test.h"
#include "thread_safe_wrapper_test.h"

#include "third/letter.h"
#include "timer_test.h"

#include <net/proto/net_archive.h>

#define AS_MEMBER()

template<class Tuple, std::size_t N>
struct TuplePrinter {
    static void output(std::stringstream& str, const Tuple& t, const char* memb_arr[])
    {
        TuplePrinter<Tuple, N - 1>::output(t);
        str << ", " << memb_arr[N-1] << "=" << std::get<N - 1>(t);
    }
};

template<class Tuple>
struct TuplePrinter<Tuple, 1>{
    static void output(std::stringstream& str, const Tuple& t, const char* memb_arr[])
    {
        //std::cout << "ele type: " << typeid(std::get<0>(t)).name() << std::endl;
        if (std::is_pointer<std::tuple_element<0, Tuple>::type>::value)
        {
            str << memb_arr[0] << "=" << *std::get<0>(t);
        }
        else
        {
            str << memb_arr[0] << "=" << std::get<0>(t);
        }
    }
};



#define de_type(val)\
    typedef std::tuple<decltype(&val)> t_type;\
    t_type t_val;           \
    std::get<0>(t_val) = &val;

#define make_stats_mgr(mgrname, val)  \
class mgrname                                                  \
{                                                              \
    mgrname()                                                  \
    {                                                          \
          std::get<0>(t_val) = &val;                           \
          member_name_arr[0] = #val;                           \
    }                                                          \
public:                                                        \
                                                               \
    static std::string info()                                  \
    {                                                          \
        return instance()->output();                           \
    }                                                          \
                                                               \
    static mgrname* instance()                                 \
    {                                                          \
        static mgrname _instance;                              \
        return &_instance;                                      \
    }                                                           \
protected:                                                      \
    std::string output()                                        \
    {                                                           \
        std::stringstream stream_;                              \
        TuplePrinter<t_type, tuple_size<t_type>::value>   \
        ::output(stream_, t_val, member_name_arr);        \
        return stream_.str();                                   \
    }               \
                        \
protected:                                                      \
    typedef std::tuple<decltype(&val)> t_type;                  \
    t_type t_val;                                               \
    const char* member_name_arr[10];                                        \
};

std::string aaadsd = "9999";
make_stats_mgr(test_mgr, aaadsd);


int _tmain(int argc, _TCHAR* argv[])
{
    std::cout << test_mgr::info() << std::endl;

    printf("\r\n");

    printf("Running main() from ut.cpp\n");
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();

	return 0;
}

