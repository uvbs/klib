#pragma once

template<typename T , typename TBase> class TIsDerived
{
public:
    static int t(TBase* base)
    {
        return 1;
    }
    static  char t(void* t2)
    {
        return 0;
    }

    enum 
    {
        Result = ( sizeof(int) == sizeof(t( (T*)NULL) )  ), 
    };
};