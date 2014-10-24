#ifndef _klib_item_counter__h_
#define _klib_item_counter__h_

#include <map>


template<class T>
class item_counter
{
public:
    size_t size() 
    {
        return counter_.size();
    }

    size_t add(const T& t)
    {
        auto itr = counter_.find(t);
        if (itr == counter_.end())
        {
            counter_[t] = 1;
            return 1;
        }
        else 
        {
            itr->second += 1;
            return itr->second;
        }
    }

    size_t dec(const T& t)
    {
        auto itr = counter_.find(t);
        if (itr == counter_.end())
        {
            return 0;
        }
        else 
        {
            itr->second -= 1;
            if (0 == itr->second)
            {
                counter_.erase(itr);
                return 0;
            }

            return itr->second;
        }
    }
    
protected:
    std::map<T, size_t>   counter_;
};






#endif // _klib_item_counter__h_