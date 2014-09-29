#include "sign_helper.h"


#include <xyssl/rsa.h>
#include <xyssl/sha1.h>


#define get_ptr(p) ((rsa_context*) p)


sign_helper::sign_helper(void)
{
    rsa_ctx_ = nullptr;
    rsa_ctx_ = new rsa_context;
}

sign_helper::~sign_helper(void)
{
    delete (rsa_context*) this->rsa_ctx_;
}

bool sign_helper::set_sign_info(const std::string& n,
    const std::string& e,
    const std::string& d,
    const std::string& p,
    const std::string& q,
    const std::string& dp,
    const std::string& dq,
    const std::string& qp)
{
    int ret;
    auto ptr = get_ptr(this->rsa_ctx_);

    rsa_init(ptr, RSA_PKCS_V15, 0, NULL, NULL );

    if( ( ret = mpi_read_string( &ptr->N , 16, (char*) n.c_str()) ) != 0 ||
        ( ret = mpi_read_string( &ptr->E , 16, (char*) e.c_str()) ) != 0 ||
        ( ret = mpi_read_string( &ptr->D , 16, (char*) d.c_str()) ) != 0 ||
        ( ret = mpi_read_string( &ptr->P , 16, (char*) p.c_str()) ) != 0 ||
        ( ret = mpi_read_string( &ptr->Q , 16, (char*) q.c_str()) ) != 0 ||
        ( ret = mpi_read_string( &ptr->DP, 16, (char*) dp.c_str()) ) != 0 ||
        ( ret = mpi_read_string( &ptr->DQ, 16, (char*) dq.c_str()) ) != 0 ||
        ( ret = mpi_read_string( &ptr->QP, 16, (char*) qp.c_str()) ) != 0 )
    {
        printf( " failed\n  ! mpi_read_file returned %d\n\n", ret );
        return false;
    }
    
    ptr->len = ( mpi_msb( &ptr->N ) + 7 ) >> 3;
    return true;
}

bool sign_helper::sign_content(unsigned char* buff, int nlen, std::string& str_sign)
{
    int ret;
    unsigned char buf[512];
    unsigned char hash[20];
    sha1(buff, nlen, hash);
    auto ptr = get_ptr(this->rsa_ctx_);

    if( ( ret = rsa_pkcs1_sign(ptr, RSA_PRIVATE, RSA_SHA1,
        20, hash, buf ) ) != 0 )
    {
        printf( " failed\n  ! rsa_pkcs1_sign returned %d\n\n", ret );
        return false;
    }

    str_sign = std::string((const char*)buf, ptr->len);

    return true;
}