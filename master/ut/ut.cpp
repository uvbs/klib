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

#pragma comment(lib, "statics")

//#pragma comment(lib, "ws2_32")

#include <kthread/atomic.h>
#include <klib_link.h>
#include <core/uuid.h>
#include <core/timestamp.h>
#include <pattern/ringbuffer.h>

#include "lazy_test.h"
#include "cache_lru_test.h"
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
#include "sunday_test.h"
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
#include "log_helper_test.h"
#include "tracer_test_h.h"

#include "third/letter.h"
#include "timer_test.h"
#include "ac_wrapper_test.h"
#include "stats_test.h"

//#include "unzip_test.h"

#include <net/proto/net_archive.h>


#include <pattern/ref_buffer.h>
#include <pattern/res_mgr.h>



class TheRes : public spicify_res
{
};

class aaa_res_mgr 
    : public spicify_res_mgr<TheRes>
    , public singleton<aaa_res_mgr>
{
};

int _tmain(int argc, _TCHAR* argv[])
{
    ref_buffer<1024>* ref = new ref_buffer<1024>;
    KLIB_ASSERT(ref->size() == ref_buffer<1024>::e_buff_size);
    scope_buff<ref_buffer<1024>> ref_scp(ref);
    ref->release();

    ref_buff* ptr = ref_buff::create(200);
    scope_buff<ref_buff> scp(ptr);
    scp->get_buffer();

    ptr->release();

    TheRes* res = aaa_res_mgr::instance()->create();
    res->release();

    res_mgr_imp aaa;
    aaa.reg(aaa_res_mgr::instance());

    klib::pattern::ring_buffer<int> rbuff;
    rbuff.push_back(1);

    printf("\r\n");

    printf("Running main() from ut.cpp\n");
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();

	return 0;
}

