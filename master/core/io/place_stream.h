#ifndef _klib_place_streaam_h_
#define _klib_place_streaam_h_


namespace klib {
namespace io {


template<typename t_host_type>
class place_stream
{
public:
    typedef t_host_type*  pointer;

public:
    place_stream() {}
    place_stream(pointer p, size_t len ) : buff_(p), len_(len) {}

    int read();

protected:
    pointer buff_;
    size_t  len_;
    size_t  pos_;
};


}}


#endif // _klib_place_streaam_h_