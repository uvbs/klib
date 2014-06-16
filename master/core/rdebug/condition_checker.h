


VConditionTriger::VConditionTriger(void)  
{  
    m_bSetOnce =false;  
    m_nCount=0;  
    m_nLastTime=0;    
}  


VConditionTriger::~VConditionTriger(void)  
{  

}  


// bool VConditionTriger::CheckEqualValue(ConditionType tContion)  
// {  
//   
//  return false;  
// }  


bool VConditionTriger:: CheckOnce()  
{  
    bool bRet= false;  
    m_lock.lock();  
    {  
        if (!m_bSetOnce)  
        {  
            m_bSetOnce=true;  
            bRet =true;  
        }   
    }  
    m_lock.unlock();  
    return bRet;  
}  

bool VConditionTriger::ClearOnce()  
{  
    bool bRet= false;  
    m_lock.lock();  
    {  
        m_bSetOnce=false;  
        bRet =true;  
    }  
    m_lock.unlock();  
    return bRet;  
}  

uint64 VConditionTriger::IncreaseCount()  
{  
    m_lock.lock();  
    uint64 nRet =IncreaseSequence(m_nCount);  
    m_lock.unlock();  
    return nRet;  
}  

bool VConditionTriger::CheckCountInterval(uint64 nCount,bool bIncreaseCount)  
{  
    if (nCount==0)  
    {  
        return false;  
    }  
    bool bRet= false;  
    if (m_nCount%nCount==0)  
    {  
        bRet= true;  
    }  
    else  
    {  
        bRet = false;  
    }  
    if (bIncreaseCount)  
    {  
        IncreaseCount();  
    }  
    return bRet;  
}  

bool VConditionTriger::CheckTimeInterval(uint64 nTime)  
{  
    FILETIME ft;  
    GetSystemTimeAsFileTime(&ft);  
    LARGE_INTEGER li = *(LARGE_INTEGER*)&ft;  
    uint64 current_time  = li.QuadPart;  
    bool bRet =false;  
    m_lock.lock();  
    if (fabs(long double(current_time - m_nLastTime))>=nTime)  
        //if (current_time%nTime==0)  
    {  
        m_nLastTime = current_time;  
        bRet= true;  
    }  
    else  
    {  
        bRet= false;  
    }  
    m_lock.unlock();  
    return bRet;  
}  

bool VConditionTriger::CheckKeydown(int nKey1,int nKey2,int nKey3)  
{  
    if (((GetKeyState(nKey1) & (1 << (sizeof(SHORT)*8-1))) != 0 ))  
    {  
        if (((GetKeyState(nKey2) & (1 << (sizeof(SHORT)*8-1))) != 0 ))  
        {  
            if (((GetKeyState(nKey3) & (1 << (sizeof(SHORT)*8-1))) != 0 ))  
            {  
                return true;  
            }  
        }  
    }  
    return false;  
}  

uint64 VConditionTriger::IncreaseSequence(uint64& nSequence)  
{  
    if(nSequence == 0xFFFF)  
    {  
        nSequence   = 1;  
    }else{  
        nSequence   ++;  
    }  
    return nSequence;  
}