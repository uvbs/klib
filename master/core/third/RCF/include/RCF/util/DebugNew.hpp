
//*****************************************************************************
// SF - Serialization Framework
// Copyright (c) 2003. All rights reserved.
// Developed by Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//*****************************************************************************

//*****************************
// DEPRECATED!!!
//*****************************

// Overriding the new operator doesn't sit well with Managed C++
#if defined(_MSC_VER) && defined(_MANAGED)
#error Shoudnt use DebugNew with Managed C++
#endif

#ifndef _UTIL_DEBUGNEW_HPP_
#define _UTIL_DEBUGNEW_HPP_

#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>

#include <iostream>

#include "Meta.hpp"
#include "ThreadLibrary.hpp"
#include "Platform/Platform.hpp"

#ifdef new
#error "new" has already been #defined!!!
#endif

static void print(const char *sz) { Platform::OS::OutputDebugString(sz); }

// Memory allocations are stored in a C-style linked list, using malloc/free.
// NB!!! The storage of allocation info must not involve any calls to new or delete,
//         that would lead to a very precarious state of affairs... For this reason
//         we can't use STL containers to store the information.
class Allocations {
public:
    static void Add( void *pv, int nSize, bool bArray, const char *szFile, int nLine)
    {
        Platform::Threads::mutex::scoped_lock lock( getMutex() );

        AllocInfo *pAlloc = (AllocInfo *) malloc( sizeof(AllocInfo) );
        pAlloc->pv_ = pv;
        pAlloc->nSize_ = nSize;
        pAlloc->szFile_ = szFile;
        pAlloc->nLine_ = nLine;
        pAlloc->bArray_ = bArray;
        pAlloc->pNext_ = getAllocations();

        getAllocations() = pAlloc; 
    }
    static void Remove( void *pv, bool bArray, const char *szFile, int nLine)
    {
        Platform::Threads::mutex::scoped_lock lock( getMutex() );

        if (pv) {
            AllocInfo *pAlloc = NULL;
            if (getAllocations() == NULL) {
                RegisterInvalidDelete(pv, szFile, nLine);
            }
            else if (getAllocations()->pv_ == pv) {
                pAlloc = getAllocations();
                getAllocations() = getAllocations()->pNext_;

                if (pAlloc->bArray_ != bArray)
                    RegisterMismatch(pAlloc, szFile, nLine);
                
                free(pAlloc);
            }
            else {
                pAlloc = getAllocations();
                while (pAlloc->pNext_ && pAlloc->pNext_->pv_ != pv)
                    pAlloc = pAlloc->pNext_;
                
                if (pAlloc->pNext_) {
                    AllocInfo *pTemp = pAlloc->pNext_;
                    pAlloc->pNext_ = pAlloc->pNext_->pNext_;

                    if (pTemp->bArray_ != bArray)
                        RegisterMismatch(pTemp, szFile, nLine);
                    
                    free(pTemp);
                }
                else {
                    RegisterInvalidDelete(pv, szFile, nLine);
                }
            }
        }
    }

    static void Reset()
    {
        Platform::Threads::mutex::scoped_lock lock( getMutex() );

        AllocInfo *pAlloc = getAllocations();
        getAllocations() = NULL;
        while (pAlloc) {
            AllocInfo *pTemp = pAlloc->pNext_;
            free(pAlloc);
            pAlloc = pTemp;
        }
        Mismatch *pMismatch = getMismatches();
        getMismatches() = NULL;
        while (pMismatch) {
            Mismatch *pTemp = pMismatch->pNext_;
            free(pMismatch);
            pMismatch = pTemp;
        }
    }
    
    static void Dump()
    {
        Platform::Threads::mutex::scoped_lock lock( getMutex() );

        print("***************************************\n");
        print("Contents of debug memory manager:\n");
        
        print("Outstanding allocations:\n");
        AllocInfo *pAlloc = getAllocations();
        if (pAlloc) {
            while (pAlloc) {
                char szBuffer[1024];
                sprintf(szBuffer, "%s(%i)\n", pAlloc->szFile_, pAlloc->nLine_);
                print(szBuffer);
            
                pAlloc = pAlloc->pNext_;
            }
        }

        print("Mismatched allocations/deallocations:\n");
        Mismatch *pMismatch = getMismatches();
        if (pMismatch) {
            while(pMismatch) {
                char szBuffer[1024];
                sprintf(szBuffer, "%s(%i) : wrong delete/delete[] was called\n", pMismatch->szFile1_, pMismatch->nLine1_ );
                print(szBuffer);
                pMismatch = pMismatch->pNext_;
            }
        }
        // NOTE: the InvalidDelete list usually contains a lot of spurious "invalid deletes", since the memory manager
        //         traps delete operations by the std library, that for some unknown reason do not have matching new operations.

        //InvalidDelete *pInvalidDelete = getInvalidDeletes();
        //if (pInvalidDelete) {
        //    print("Invalid deletes:\n");
        //    while(pInvalidDelete) {
        //        char szBuffer[1024];
        //        //sprintf(szBuffer, "%s(%i) : invalid delete (ptr=%p)\n", pInvalidDelete->szFile_, pInvalidDelete->nLine_, pInvalidDelete->pv_ );
        //        sprintf(szBuffer, "invalid delete (ptr=%p)\n", pInvalidDelete->pv_ );
        //        print(szBuffer);
        //        pInvalidDelete = pInvalidDelete->pNext_;
        //    }
        //}
        print("***************************************\n");
    }

private:

    static Platform::Threads::mutex &getMutex() {
        static Platform::Threads::mutex *mutex = (Platform::Threads::mutex *) malloc( sizeof(Platform::Threads::mutex) );
        new (mutex) Platform::Threads::mutex;
        return *mutex;
    }

    struct AllocInfo {
        void *pv_;
        int nSize_;
        const char *szFile_;
        int nLine_;
        bool bArray_;
        
        AllocInfo *pNext_;
    };

    struct Mismatch {
        const char *szFile1_;
        int nLine1_;
        const char *szFile2_;
        int nLine2_;

        Mismatch *pNext_;
    };

    struct InvalidDelete {
        void *pv_;
        const char *szFile_;
        int nLine_;
        
        InvalidDelete *pNext_;
    };

    static AllocInfo *&getAllocations() 
    {
        static AllocInfo *pAlloc = NULL;
        return pAlloc;
    }
    static Mismatch *&getMismatches()
    {
        static Mismatch *pMismatch = NULL;
        return pMismatch;
    }
    static InvalidDelete *&getInvalidDeletes()
    {
        static InvalidDelete *pInvalidDelete = NULL;
        return pInvalidDelete;
    }
    static void RegisterMismatch(AllocInfo *pAlloc, const char *szFile, int nLine)
    {
        Mismatch *pMismatch = (Mismatch *) malloc( sizeof(Mismatch) );
        pMismatch->szFile1_ = pAlloc->szFile_;
        pMismatch->nLine1_ = pAlloc->nLine_;
        pMismatch->szFile2_ = szFile;
        pMismatch->nLine2_ = nLine;
        pMismatch->pNext_ = getMismatches();
        getMismatches() = pMismatch;
    }
    static void RegisterInvalidDelete(void *pv, const char *szFile, int nLine)
    {
        InvalidDelete *pInvalidDelete = (InvalidDelete *) malloc( sizeof(InvalidDelete) );
        pInvalidDelete->pv_ = pv;
        pInvalidDelete->szFile_ = szFile;
        pInvalidDelete->nLine_ = nLine;
        pInvalidDelete->pNext_ = getInvalidDeletes();
        getInvalidDeletes() = pInvalidDelete;
    }

};

class DisplayLeaks {
public:
    bool &deinit() { static bool bDeinit = false; return bDeinit; }
    ~DisplayLeaks() { if (!deinit()) { Allocations::Dump(); Allocations::Reset(); deinit() = true; } }
};
static DisplayLeaks DisplayLeaksInst;


inline void *debug_new(unsigned int size, const char *file, int line, bool bArray = false)
{
    void *ptr = (void *)malloc(size);
    Allocations::Add( ptr, size, bArray, file, line );
    return(ptr);
}

inline void debug_delete(void *ptr, bool bArray = false, const char *file = NULL, int line = -1)
{
    Allocations::Remove( ptr, bArray, file, line );
    free(ptr);
}

inline void *operator new(unsigned int size, const char *file, int line)
{ return debug_new(size, file, line); }

inline void *operator new[](unsigned int size, const char *file, int line)
{ return debug_new(size, file, line, true); }

inline void operator delete(void *ptr) throw ()
{ try { debug_delete( ptr ); } catch(...) {} }

inline void operator delete[](void *ptr) throw()
{ try { debug_delete( ptr, true ); } catch(...) {} }

#define SF_UTIL_DEBUG_NEW new(__FILE__, __LINE__)

#endif // ! _UTIL_DEBUGNEW_HPP_
