#ifndef _klib_find_tool_test_h
#define _klib_find_tool_test_h

#include <algo/search/find_tool.h>

TEST(FindToolTest, t1)
{
    klib::core::WideFindTool d(L"GET_buffer_sdfsdf_strings_lsjflsjfl__POST");
    _ASSERT(wcscmp(d->endptr(4), L"POST") == 0);


    //EXPECT_EQ(d->find(L"GET"), NULL);
    //wcout << d->find(L"GET", 3) << endl;
    //wcout << d->find(L"GET_buffer_sdfsdf_strings_lsjflsjfl__POST", 41, L"buffer_", 7) << endl;
    ASSERT_TRUE(d->find(L"buffer_", (size_t)0) == NULL);
    ASSERT_TRUE(d->find(L"buffer=", nullptr) == NULL);
    ASSERT_TRUE(d->find(L"buffer=", 7) == NULL);

    ASSERT_TRUE(d->find(L"strings") != NULL);
    ASSERT_TRUE(d->find(L"CHAR", 4, L"CHAR", 4) != NULL);
    ASSERT_TRUE(d->find(L"CHAR", 4, L"char", 4, klib::core::WideFindTool::emCaseSensitve) == NULL);
    ASSERT_TRUE(d->find(L"CHAR", 4, L"char", 4, klib::core::WideFindTool::emCaseInsensitive) != NULL);

    ASSERT_TRUE(d->beginwith(L"GET"));
    ASSERT_TRUE(d->beginwith(L"GET", 2));
    ASSERT_TRUE(d->endwith(L"POST"));
    ASSERT_TRUE(d->endwith(L"POST", 4));


    klib::core::MutiFindTool x("__sdfsdF__dsfsdfs__endx");
    ASSERT_TRUE(strcmp(x->endptr(4), "endx") == 0);

    ASSERT_TRUE(x->find("sdfsdF") != NULL);
    ASSERT_TRUE(x->find("sdfsdF", 5) != NULL);
    ASSERT_TRUE(x->find("hello world!!!!!", 16, "world", 5) != NULL);
    ASSERT_TRUE(x->find("CHAR", 4, "CHAR", 0) == NULL);
    ASSERT_TRUE(x->find("CHAR", 4, "CHAR", 4) != NULL);
    ASSERT_TRUE(x->find("CHAR", 4, "char", 4, klib::core::MutiFindTool::emCaseSensitve) == NULL);
    ASSERT_TRUE(x->find("CHAR", 4, "char", 4, klib::core::MutiFindTool::emCaseInsensitive) != NULL);

    ASSERT_TRUE(x->beginwith("__"));
    ASSERT_TRUE(x->beginwith("__", 2));
    ASSERT_TRUE(false == x->beginwith(""));
    ASSERT_TRUE(false == x->beginwith("d__"));
    ASSERT_TRUE(false == x->beginwith("d__", 2));

    ASSERT_TRUE(x->endwith("endx"));
    ASSERT_TRUE(x->endwith("endx", 4));
    ASSERT_TRUE(false == x->endwith("a_endx"));
    ASSERT_TRUE(false == x->endwith("a_endx", 6));
}

#endif
