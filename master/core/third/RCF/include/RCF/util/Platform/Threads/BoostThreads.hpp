
#ifndef _UTIL_PLATFORM_THREADS_BOOSTTHREADS_HPP_
#define _UTIL_PLATFORM_THREADS_BOOSTTHREADS_HPP_

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4275 ) // warning C4275: non dll-interface class 'std::runtime_error' used as base for dll-interface class 'boost::thread_resource_error'
#pragma warning( disable : 4251 ) // warning C4251: 'boost::thread_group::m_threads' : class 'std::list<_Ty>' needs to have dll-interface to be used by clients of class 'boost::thread_group'
#endif

#include <boost/thread/thread.hpp>
#include <boost/thread/tss.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/read_write_mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/condition.hpp>

#ifdef _MSC_VER
#pragma warning( pop )
#endif

namespace Platform {

    namespace Threads {

        typedef boost::thread thread;
        typedef boost::thread_group thread_group;

        template<typename T>
        struct thread_specific_ptr {
            typedef boost::thread_specific_ptr<T> Val;
        };

        typedef boost::mutex                    mutex;
        typedef boost::try_mutex                try_mutex;
        typedef boost::recursive_mutex          recursive_mutex;
        typedef boost::recursive_try_mutex      recursive_try_mutex;
        typedef boost::condition                condition;

        enum read_write_scheduling_policy { 
            writer_priority = boost::read_write_scheduling_policy::writer_priority, 
            reader_priority = boost::read_write_scheduling_policy::reader_priority, 
            alternating_many_reads = boost::read_write_scheduling_policy::alternating_many_reads, 
            alternating_single_read = boost::read_write_scheduling_policy::alternating_single_read 
        };

        class read_write_mutex : public boost::read_write_mutex
        {
        public:
            read_write_mutex(read_write_scheduling_policy policy) : 
                boost::read_write_mutex(
                    boost::read_write_scheduling_policy::read_write_scheduling_policy_enum(policy))
            {}
        };

    } // namespace Threads

} // namespace Platform

#endif // ! _UTIL_PLATFORM_THREADS_BOOSTTHREADS_HPP_
