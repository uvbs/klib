
#pragma once

#include <core/array.h>
#include <stdlib.h>

TEST(array_test, 1)
{
    if (true) 
    {
        klib::array_1d<int, 10> arr;

        arr.assign(0);

        auto itr = arr.begin();
        for (; itr != arr.end(); ++ itr)
        {
            printf("%d\t", *itr);
        }

    }
    
    if (true) 
    {
        klib::array_2d<int, 10, 10> arr;

        arr.assign(100);

        auto itr = arr.begin();
        for (; itr != arr.end(); ++itr)
        {
            printf("%d\t", *itr);
        }

    }
    
    if (true) 
    {
        klib::array_3d<int, 3, 3, 3> arr;

        arr.assign(33);

        auto itr = arr.begin();
        for (; itr != arr.end(); ++itr)
        {
            printf("%d\t", *itr);
        }
    }

    printf("\n");
}