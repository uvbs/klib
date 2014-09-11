#ifndef  _klib_ac_wrapper_h_
#define _klib_ac_wrapper_h_


#include <functional>

namespace klib{
namespace algo{

typedef std::function<int(void * id, void *tree, int index, void *neg_list)>  
    ac_match_callback;

// wrapper for ac multi pattern 
class ac_wrapper
{
public:
    ac_wrapper() ;
    ~ac_wrapper() ;

    bool add_pattern(const char* pt_buf, int pt_len, void*data);
    bool compile();
    int  size();

    void set_callback(ac_match_callback call);
    bool search(const char* buf, int buflen);

protected:
    static int Match(void * id, void *tree, int index, void *data, void *neg_list);

protected:
    void*              handle_;
    ac_match_callback  callback_;
};



}}



#endif // _klib_ac_wrapper_h_