
#ifndef _klib_bitmap_test_h
#define _klib_bitmap_test_h

#include <algo/bitmap.h>

TEST(bitmap, 1)
{
    int a[] = {5,8,7,6,3,1,10,78,56,34,23,12,43,54,65,76,87,98,89,100};  
    int i; 

    klib::algo::bitmap bmp;
    bmp.init(100, 0);

    for(i=0; i<20; i++)
    {
        bmp.set(a[i]);  
    }
    for(i=0; i<=100; i++)  
    {  
        if(bmp.get(i) > 0 )  
            cout << bmp.data(i)<< " ";
    }  
    cout << endl; 
}

#endif
