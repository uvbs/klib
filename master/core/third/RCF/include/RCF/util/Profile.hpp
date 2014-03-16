
//*****************************************************************************
// Copyright (c) 2003. All rights reserved.
// Developed by Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//*****************************************************************************

#ifndef _UTIL_PROFILE_HPP_
#define _UTIL_PROFILE_HPP_

#include <sys/types.h>
#include <sys/timeb.h>

#include <map>
#include <memory>
#include <sstream>
#include <vector>

#include "AutoRun.hpp"
#include "ThreadLibrary.hpp"
#include "Trace.hpp"

namespace util {

    class ProfilingResults 
    {
    private:
        ProfilingResults() : profileTraceChannel("Profile") 
        {
            profileTraceChannel.setTraceTarget("stdout");
        }
    public:
        static ProfilingResults &getSingleton()
        {
            static ProfilingResults *pProfilingResults = new ProfilingResults;
            return *pProfilingResults;
        }
        static void deleteSingleton()
        {
            delete &getSingleton();
        }
        void dump()
        {
            Platform::Threads::mutex::scoped_lock lock(mMutex);
            for (unsigned int i=0; i<mResults.size(); i++) 
            {
                std::ostringstream ostr;
                ostr << "*************************\n";
                ostr << "Profiling results for thread #" << i << ":\n";
                
                for (DataT::iterator iter_i = mResults[i]->begin(); iter_i != mResults[i]->end(); iter_i++) {
                    if (!(*iter_i).first.empty()) {
                        ostr << (*iter_i).first << ": " << (*iter_i).second.first / 1000.0 << " s.\n";
                        SubDataT &subData = (*iter_i).second.second;
                        for (SubDataT::iterator iter_j = subData.begin(); iter_j != subData.end(); iter_j++)
                            ostr << "    " << (*iter_j).first << ": " << (*iter_j).second / 1000.0 << "s.\n";
                    }
                }

                ostr << "*************************\n";
                profileTraceChannel.trace(ostr.str());
            }
        }

    private:
        friend class ProfilingData;
        typedef std::map<std::string, unsigned int> SubDataT;
        typedef std::map<std::string, std::pair<unsigned int, SubDataT > > DataT;

        Platform::Threads::mutex mMutex;
        std::vector< DataT * > mResults;
        TraceChannel profileTraceChannel;

        void add( DataT *data ) 
        {
            Platform::Threads::mutex::scoped_lock scoped_lock( mMutex );
            mResults.push_back( data ); 
        }

    };

    class ProfilingData 
    {
    private:
        ProfilingData() : stack_(100), data_(new DataT()) { ProfilingResults::getSingleton().add( data_ );  }
    public:
        static ProfilingData &getThreadSpecificInstance() 
        {
            static Platform::Threads::thread_specific_ptr<ProfilingData>::Val profilingData;

            if (NULL == profilingData.get()) {
                profilingData.reset(new ProfilingData());
                profilingData->push("");
            }
            return *profilingData;
        }

        void push(std::string sz) 
        { 
            stack_.push_back(sz); 
        }

        void pop() 
        { 
            stack_.pop_back(); 
        }

        void add(unsigned int timespan)
        {
            std::string cur = stack_[stack_.size()-1];
            std::string prev = stack_[stack_.size()-2];
            (*data_)[cur].first += timespan;
            (*data_)[prev].second[cur] += timespan;
        }
        
    private:
        typedef ProfilingResults::DataT DataT;
        std::vector<std::string> stack_;
        DataT *data_;
    };

    inline unsigned int getTickCount() 
    {
        struct timeb tp;
        ftime(&tp);
        return static_cast<unsigned int>( 1000*(tp.time&0x0000ffff) + tp.millitm );
    }

    class Profile 
    {
    public:
        Profile(const std::string &name) : name(name), t0(getTickCount())
        {
            ProfilingData::getThreadSpecificInstance().push(name);
        }

        ~Profile()
        {
            ProfilingData::getThreadSpecificInstance().add(getTickCount() - t0);
            ProfilingData::getThreadSpecificInstance().pop();
        }

    private:
        std::string name;
        int t0;
    };

    UTIL_ON_INIT_NAMED( TraceManager::getSingleton(); ProfilingResults::getSingleton(); , ProfilingResultsInitialize)
    UTIL_ON_DEINIT_NAMED( ProfilingResults::getSingleton().dump(); ProfilingResults::deleteSingleton();,  ProfilingResultsDeinitialize)

} // namespace util

//#define PROFILE(desc) util::Profile profile(desc)

#endif //! _UTIL_PROFILE_HPP_
