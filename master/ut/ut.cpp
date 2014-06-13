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

#include "third/letter.h"
#include "timer_test.h"

#include <net/proto/net_archive.h>

int _tmain(int argc, _TCHAR* argv[])
{
    printf("Running main() from gtest_main.cc\n");
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();

	return 0;
}

