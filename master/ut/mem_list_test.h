#ifndef _klib_mem_list_test_h
#define _klib_mem_list_test_h



#include <mem/mem_list.h>

using namespace klib::mem;

TEST(mem_list_test, 1)
{
    mem_block_lsit<1024, 45, TRUE> s;

    s.init();
    size_t nsize = s.item_size();

    std::list<void*> v_list;

    for (size_t i=0; i<s.item_size(); ++i) 
    {
        auto ptr = s.get();
        v_list.push_back(ptr);

        EXPECT_TRUE(ptr != nullptr);
    }


    auto ptr = s.get();
    EXPECT_TRUE(ptr == nullptr);

    for (auto itr = v_list.begin(); itr != v_list.end(); ++ itr) 
    {
        s.free_(*itr);
    }
    EXPECT_TRUE(s.free_size() == nsize);


    {
        mem_block_lsit<1,1,FALSE> d_list;

        EXPECT_EQ(d_list.item_size(), 0);

        auto ptr = d_list.get();
        EXPECT_TRUE(ptr != nullptr);
        EXPECT_TRUE(d_list.used_size() == 1);
        EXPECT_TRUE(d_list.free_size() == 0);
        EXPECT_TRUE(d_list.item_size() == 1);

        d_list.free_(ptr);
        EXPECT_TRUE(d_list.used_size() == 0);
        EXPECT_TRUE(d_list.free_size() == 1);
        EXPECT_TRUE(d_list.item_size() == 1);

        EXPECT_TRUE(d_list.buffser_size() == d_list.BLOCK_SIZE);
        EXPECT_TRUE(d_list.buffser_size() == d_list.self_type::BLOCK_SIZE);
    }
}

#endif
