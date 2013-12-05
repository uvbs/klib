
const INT C1 = 52485;
const INT C2 = 27219;
const INT KEY = 75297;

class encrypt
{
public:

    bool Decrypt(BYTE* source, BYTE* destination, DWORD length)
    {
        INT Key = KEY;
        if(!source || !destination || length<=0) return FALSE;

        for (size_t i=0; i<length; ++i)
        {
            destination[i] = source[i]^Key>>8;
            //按照Key值进行1Byte的平方,推8bit

            Key = (destination[i] + Key) * C1 + C2;
            //在下一Byte中，通过C1、C2修改所要使用的Key
        }

        return TRUE;
    }

    bool Encrypt(BYTE* source, BYTE* destination, DWORD length)
    {
        BYTE PreviousBlock;
        INT Key = KEY;
        if(!source || !destination || length<=0) return FALSE;

        for (size_t i=0; i<length; ++i)
        {
            PreviousBlock = source[i];
            destination[i] = source[i]^Key>>8;
            Key = (PreviousBlock+Key) * C1 + C2;
        }
        return TRUE;
    }




};