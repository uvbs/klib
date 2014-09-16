#ifndef _sstring_h_
#define _sstring_h_


// define section sstring 
struct sstring
{
    sstring()
        : ptr(nullptr)
        , len(0)
    {}

    char*   ptr;
    size_t  len;
};


#endif // _sstring_h_