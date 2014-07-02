
#ifndef _klib_bitmap_test_h
#define _klib_bitmap_test_h

#include <algo/bitmap.h>

TEST(bitmap, 1)
{
    int test_arr[] = {5,8,7,6,3,1,10,78,56,34,23,12,43,54,65,76,87,98,89,100,88,444,783,235,983,999};
    int i;

    klib::algo::bitmap bmp;
    bmp.init(1000, 0);

    for(i=0; i<_countof(test_arr); i++)
    {
        bmp.set(test_arr[i]);
    }
    for (i=0; i<=1000; i++)
    {  
        if(bmp.get(i) > 0 )
        {
            auto end_ptr = test_arr+_countof(test_arr);
            auto itr = std::find(test_arr, end_ptr, i);
            EXPECT_TRUE(itr != end_ptr);
            EXPECT_EQ(*itr, i);
            cout << bmp.data(i) << " ";
        }
    }

    EXPECT_TRUE(bmp.set(0) == true);
    EXPECT_TRUE(bmp.set(1) == true);
    EXPECT_TRUE(bmp.set(999) == true);
    EXPECT_TRUE(bmp.set(1000) == false);
    EXPECT_TRUE(bmp.set(1001) == false);

    cout << endl; 
}

#endif
