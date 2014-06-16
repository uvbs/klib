#pragma once

#include <kthread/thread_safe_wrapper.h>


TEST(thread_safe_wrapper, 1)
{

    typedef ThreadSafeWrapper<std::list<int>> safe_list;

    safe_list lst;

    if (true) 
    {
        lst->push_back(1);
        EXPECT_EQ(lst->size(), 1);

        lst->clear();
        EXPECT_EQ(lst->size(), 0);

        for (int i=0; i<10; ++ i)
        {
            lst->push_back(i);
        }
    }

    if (true) 
    {
        auto& mutex_ = lst._mutex();
        MutexGuard guard(mutex_);

        for (auto itr=lst._wrapped()->begin(); 
            itr != lst._wrapped()->end(); 
            ++ itr)
        {
            printf("%d", *itr);
        }
        printf("\n");
    }
    
}