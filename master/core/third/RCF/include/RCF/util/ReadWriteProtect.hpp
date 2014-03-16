
//*****************************************************************************
// Copyright (c) 2003. All rights reserved.
// Developed by Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//*****************************************************************************

#ifndef _UTIL_READWRITEPROTECT_HPP_
#define _UTIL_READWRITEPROTECT_HPP_

#include "ThreadLibrary.hpp"

namespace util {

    // Utility for applying the readers/writer synchronization pattern.
    // Currently no bias for writers over readers.
    // To give multiple read access and single write access to a class X:
    //
    //class X {
    //    void read() 
    //    {
    //        ReadLock lock(rwp);
    //        // ...
    //    }
    //    void write() 
    //    {
    //        WriteLock lock(rwp);
    //        // ...
    //    }
    //    ReadWriteProtect rwp;
    //}


    class ReadWriteProtect {
    public:
        typedef Platform::Threads::recursive_mutex Mutex;
        typedef Platform::Threads::condition Event;
        ReadWriteProtect() : readerCount(0) {}
        Mutex &getReadMutex() { return readMutex; }
        Mutex &getWriteMutex() { return writeMutex; }
        int &getReaderCount() { return readerCount; }
        void waitOnReadUnlock() { Mutex::scoped_lock lock( readUnlockMutex ); readUnlockEvent.wait(lock); }
        void notifyReadUnlock() { readUnlockEvent.notify_all(); }
    private:
        Mutex readMutex;
        Mutex writeMutex;
        Mutex readUnlockMutex;
        Event readUnlockEvent;
        int readerCount;
    };

    class ReadLock {
    public:
        
        ReadLock( ReadWriteProtect &rwp ) : rwp( rwp )
        {
            ReadWriteProtect::Mutex::scoped_lock lock( rwp.getReadMutex() );
            rwp.getReaderCount()++;
        }

        ~ReadLock()
        {
            {
                ReadWriteProtect::Mutex::scoped_lock lock( rwp.getReadMutex() );
                rwp.getReaderCount()--;
            }
            rwp.notifyReadUnlock();
        }

    private:

        ReadWriteProtect &rwp;

    };

    class WriteLock {
    public:

        WriteLock( ReadWriteProtect &rwp ) 
            : rwp( rwp ), 
              read_lock( rwp.getWriteMutex(), false ), 
              write_lock( rwp.getWriteMutex(), false ) 
        {
            read_lock.lock();
            while (rwp.getReaderCount() > 0) {
                read_lock.unlock();
                rwp.waitOnReadUnlock();
                read_lock.lock();
            }
            write_lock.lock();
        }

        ~WriteLock()
        {
            write_lock.unlock();
            read_lock.unlock();
        }

    private:

        ReadWriteProtect &rwp;
        ReadWriteProtect::Mutex::scoped_lock read_RCF_UNUSED_VARIABLE(lock);
        ReadWriteProtect::Mutex::scoped_lock write_RCF_UNUSED_VARIABLE(lock);

    };


} // namespace util

#endif // ! _UTIL_READWRITEPROTECT_HPP_
