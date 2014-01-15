#ifndef _klib_md5_file_h
#define _klib_md5_file_h


#include <Wincrypt.h>

namespace klib {
namespace encode{


inline bool md5_file(std::string& strDestMd5, const TCHAR* filepath)
{
    HANDLE hFile = CreateFile(filepath, GENERIC_READ, FILE_SHARE_READ,
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (!hFile) {
        printf("CreateFile(), Errno: %u\n", GetLastError());
        return false;
    }

    BOOL ret = FALSE;
    HCRYPTPROV provider = NULL;
    HCRYPTHASH crypt_hash = NULL;
    ret = CryptAcquireContext(&provider, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
    if (!ret) {
        CloseHandle(hFile);
        return false;
    }

    CryptCreateHash(provider, CALG_MD5, NULL, 0, &crypt_hash);

    while (true) {
        DWORD bytes = 0;
        BYTE data_buffer[1024];
        ReadFile(hFile, data_buffer, sizeof(data_buffer), &bytes, NULL);
        if (bytes == 0) break;
        CryptHashData(crypt_hash, data_buffer, bytes, 0);
    }

    BYTE hash_buffer[16];
    DWORD length = sizeof(hash_buffer);
    CryptGetHashParam(crypt_hash, HP_HASHVAL, hash_buffer, &length, 0);

    CryptDestroyHash(crypt_hash);
    CryptReleaseContext(provider, 0);

    char md5str[32 + 1];
    for (DWORD index = 0; index < length; ++index)
        sprintf(md5str + (index << 1), "%02X", hash_buffer[index]);

    strDestMd5 = md5str;

    CloseHandle(hFile);

    return true;
}



}}



#endif // _klib_md5_file_h