#include "verify_helper.h"


#include "xyssl/rsa.h"
#include "xyssl/sha1.h"

#define get_ptr(p) ((rsa_context*) p)


verify_helper::verify_helper()
{
    this->rsa_ctx_ = nullptr;
    this->rsa_ctx_ = new rsa_context;
}

verify_helper::~verify_helper()
{
    delete (rsa_context*) this->rsa_ctx_;
}

bool verify_helper::set_pub_info(const std::string& n, const std::string& e)
{
    auto ptr = get_ptr(this->rsa_ctx_);
    int ret;
    ret = 1;

    rsa_init(ptr, RSA_PKCS_V15, 0, NULL, NULL );

    if( ( ret = mpi_read_string( &ptr->N, 16, (char*) n.c_str()) ) != 0 ||
        ( ret = mpi_read_string( &ptr->E, 16, (char*) e.c_str()) ) != 0 )
    {
        printf( " failed\n  ! mpi_read_file returned %d\n\n", ret );
        return false;
    }

    ptr->len = ( mpi_msb( &ptr->N ) + 7 ) >> 3;

    return true;
}

bool verify_helper::verify_sign(const std::string& str_sign, const std::string& str_content)
{
    int ret;
    unsigned char hash[20];
    auto ptr = get_ptr(this->rsa_ctx_);

    if(str_sign.size() != ptr->len )
    {
        // printf( "\n  ! Invalid RSA signature format\n\n" );
        return false;
    }

    /*
     * Compute the SHA-1 hash of the input file and compare
     * it with the hash decrypted from the RSA signature.
     */
    //printf( "\n  . Verifying the RSA/SHA-1 signature" );
    //fflush( stdout );

    sha1((unsigned char*)str_content.c_str(), str_content.size(), hash );

    if( ( ret = rsa_pkcs1_verify(ptr, 
                                 RSA_PUBLIC, RSA_SHA1,
                                 20, hash, 
                                 (unsigned char*)str_sign.c_str()) ) != 0 )
    {
        // printf( " failed\n  ! rsa_pkcs1_verify returned %d\n\n", ret );
        return false;
    }

    return true;
}