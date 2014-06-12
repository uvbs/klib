#ifndef _klib_bitmap_h
#define _klib_bitmap_h



namespace klib {
namespace algo{


class bitmap
{
public:
    bitmap();
    ~bitmap();

    //功能：初始化bitmap 
    //参数： size：bitmap的大小，即bit位的个数 
    //      start：起始值 
    //返回值：0表示失败，1表示成功 
    bool init(int size, int start);

    //功能：将值index的对应位设为1 
    //index:要设的值 
    //返回值：0表示失败，1表示成功 
    bool set(int index);

    //功能：取bitmap第i位的值 
    //i：待取位 
    //返回值：-1表示失败，否则返回对应位的值 
    int get(int i) ;

    //功能：返回index位对应的值   
    int data(int index);

    //释放内存 
    void clear();


protected:
    char* bitmap_ ;  
    int size_ ;  
    int start_; 

};









}}



#endif
