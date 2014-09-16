
#ifndef _klib_queue_test_h
#define _klib_queue_test_h

#include <pattern/one_many_queue.h>

using namespace klib::pattern;

TEST(test_queue, 1)
{
    lockfree::one_many_queue<INT, 10> q;

    q.push(10);
    EXPECT_TRUE(q.size() == 1);

    for (size_t i=0; i<9; ++i) 
    {
        q.push(1);
    }
    EXPECT_TRUE(false == q.push(10));

    INT a;
    EXPECT_TRUE(q.pop(a));
    EXPECT_TRUE(q.size() == 9);

    for (size_t i=0; i<9; ++i) 
    {
        EXPECT_TRUE(q.pop(a));
    }

    EXPECT_TRUE(q.pop(a) == false);

    q.clear();

}

#endif
