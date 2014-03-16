
#include <stdlib.h>
#include <malloc.h>

#include <iostream>
#include <map>
#include <string>

#include <boost/noncopyable.hpp>
#include <boost/static_assert.hpp>

#include "AutoRun.hpp"
#include "StackTrace.hpp"

#include "Platform/Threads/BoostThreads.h"

namespace util {

    bool allocationFlagReady = true;
    typedef Platform::Threads::thread_specific_ptr<bool>::Val TSBoolPtr;
    static TSBoolPtr *pAllocationFlag;

    bool getAllocationFlagReady()
    {
        return allocationFlagReady;
    }

    void setAllocationFlagReady(bool ready)
    {
        allocationFlagReady = ready;
    }

    bool getAllocationFlag()
    {    
        if (pAllocationFlag == NULL)
        {
            setAllocationFlagReady(false);
            pAllocationFlag = new TSBoolPtr;
            setAllocationFlagReady(true);
        }
        if (pAllocationFlag->get() == NULL)
        {
            bool *pb = (bool *) malloc( sizeof(bool) );
            new (pb) bool;
            *pb = false;
            pAllocationFlag->reset( pb );
        }
        return **pAllocationFlag;
    }

    void setAllocationFlag(bool flag)
    {
        **pAllocationFlag = flag;
    }

    class MemoryCheck : public boost::noncopyable 
    {
    private:
        MemoryCheck() : checkNoMansLand(false)
        {}

        const bool checkNoMansLand;
            
    public:
        static MemoryCheck &getSingleton()
        {
            return *getSingletonPtr();
        }

        static MemoryCheck *getSingletonPtr()
        {
            static MemoryCheck *pInstance;
            if (pInstance == NULL)
            {
                pInstance = (MemoryCheck *) malloc( sizeof(MemoryCheck) );
                setAllocationFlagReady(false);
                new (pInstance) MemoryCheck;
                setAllocationFlagReady(true);
            }
            return pInstance;
        }

        static void init()
        {
            getSingleton();
            getAllocationMapMutex();
            getAllocationFlag();
        }

        void *allocate(unsigned int size, const std::string &stackTrace)
        {
            BOOST_STATIC_ASSERT( sizeof(char) == 1 );
            BOOST_STATIC_ASSERT( sizeof(int) == 4 );

            setAllocationFlag(true);

            char *ptr = NULL;

            if (checkNoMansLand)
            {
                void *vmem = malloc(size + 2*4);
                char *mem = (char *) vmem;
                char *before = mem;
                char *after = mem + 4 + size;
                ptr = mem + 4;
                *((int *) before) = 0xdeadbeef;
                *((int *) after) = 0xdeadbeef;
            }
            else
            {
                void *mem = malloc(size);
                ptr = (char *) mem;
            }

            {
                Platform::Threads::mutex::scoped_lock lock(getAllocationMapMutex());
                //assert( getAllocationMap().find(mem) == getAllocationMap().end() );
                Allocation &allocation = getAllocationMap()[ptr];
                if( allocation.first > 0 )
                {
                    int n=0;
                }
                allocation.first = size;
                allocation.second.first = false;
                allocation.second.second = stackTrace;
            }

            setAllocationFlag(false);

            return ptr;
        }

        void *allocateArray(unsigned int size, const std::string &stackTrace)
        {
            /*BOOST_STATIC_ASSERT( sizeof(char) == 1 );
            BOOST_STATIC_ASSERT( sizeof(int) == 4 );
            void *vmem = malloc(size + 2*4);
            char *mem = vmem;
            char *before = mem;
            char *obj = mem + 4;
            char *after = mem + 4 + size;
            *(int*) before) = 0xdeadbeef;
            *(int*) after) = 0xdeadbeef;
            return obj;*/
            assert(0);
        }

        void deallocate(void *ptr)
        {

            setAllocationFlag(true);

            Platform::Threads::mutex::scoped_lock lock(getAllocationMapMutex());
            //AllocationMap::iterator it = getAllocationMap().find( ((char*)ptr)-4 );
            AllocationMap::iterator it = getAllocationMap().find(ptr);
            assert( it != getAllocationMap().end() );
            Allocation &allocation = (*it).second;
            unsigned int size = allocation.first;
            bool array = allocation.second.first;
            const std::string &stackTrace = allocation.second.second;

            if (checkNoMansLand)
            {
                char *obj = (char *) ptr;
                char *before = obj - 4;
                char *after = obj + size;
                assert( *((int *) before) == 0xdeadbeef );
                assert( *((int *) after) == 0xdeadbeef );
                free(before);
            }
            else
            {
                free(ptr);
            }
            
            //getAllocationMap().erase(it);
            allocation.first = 0;
            allocation.second.first = false;
            allocation.second.second = "deleted!";

            setAllocationFlag(false);
        }

        void deallocateArray(void *ptr)
        {
            assert(0);
        }

        bool findAllocation(void *ptr)
        {
            Platform::Threads::mutex::scoped_lock lock(getAllocationMapMutex());
            //return getAllocationMap().find( ptr ) != getAllocationMap().end();
            return getAllocationMap()[ptr].first > 0;
        }

    private:

        typedef std::pair<unsigned int, std::pair<bool, std::string> > Allocation;
        typedef std::map<void *, Allocation> AllocationMap;
        AllocationMap allocationMap;

        AllocationMap &getAllocationMap()
        {
            return allocationMap;
        }
        
        static Platform::Threads::mutex &getAllocationMapMutex()
        {
            static Platform::Threads::mutex allocationMapMutex;
            return allocationMapMutex;
        }

        

    };

    

    //AUTO_RUN( MemoryCheck::init() );

} // namespace util

inline void *operator new(unsigned int size)
{
    if (!util::getAllocationFlagReady() || util::getAllocationFlag())
    {
        return malloc(size);
    }
    else
    {
        return util::MemoryCheck::getSingleton().allocate(size, util::getStackTrace());
    }
}

inline void *operator new[](unsigned int size)
{ 
    if (!util::getAllocationFlagReady() || util::getAllocationFlag())
    {
        return malloc(size);
    }
    else
    {
        return util::MemoryCheck::getSingleton().allocateArray(size, util::getStackTrace());
    }
}

inline void operator delete(void *ptr) throw ()
{
    if (!util::MemoryCheck::getSingleton().findAllocation(ptr))
    {
        free(ptr);
    }
    else
    {
        util::MemoryCheck::getSingleton().deallocate(ptr);
    }
    
}

inline void operator delete[](void *ptr) throw()
{
    if (!util::MemoryCheck::getSingleton().findAllocation(ptr))
    {
        free(ptr);
    }
    else
    {
        util::MemoryCheck::getSingleton().deallocateArray(ptr);
    }
}
