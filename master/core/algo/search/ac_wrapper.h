#ifndef  _klib_ac_wrapper_h_
#define _klib_ac_wrapper_h_


namespace klib{
namespace algo{


// wrapper for ac multi pattern 
class ac_wrapper
{
public:
    ac_wrapper() ;
    ~ac_wrapper() ;

    bool add_pattern(char* pt_buf, int pt_len, void*data);
    bool compile();
    int  size();

    bool search(const char* buf, int buflen);

protected:
    void* handle_;
};



}}



#endif // _klib_ac_wrapper_h_