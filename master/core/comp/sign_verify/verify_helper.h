#ifndef _klib_verify_helper_h
#define _klib_verify_helper_h

/**
// 示例的 n和e 
const std::string g_PublickKey_N = "BA3F7EFF8A7AD3A6A445FD8C16E373F43C45A81D23E27588E13FCB8CA8A44F6E0E60AFD2E880759FF916A11025B1682A318FF0ADB3F45B5EE559E3D402A844B5DBEF71273D21AB8CE46F775964CE80617C645DF0994274041D993257A7F90BD50F0CDC84EA6AA31C766B2EB9A9832C0D13EE59EFF745B638AA0CCBAE6A939835";
const std::string g_PublickKey_E = "010001";
*/

#include <string>

//----------------------------------------------------------------------
// 签名认证的类
class verify_helper
{
public:
    verify_helper();
    ~verify_helper();

    bool set_pub_info(const std::string& n, const std::string& e);
    bool verify_sign(const std::string& str_sign, const std::string& str_content);
protected:
    void* rsa_ctx_;
};



#endif
