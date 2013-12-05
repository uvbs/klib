#ifndef QUICK_SORT_H
#define QUICK_SORT_H

#include<random>
#include<iterator>
#include<utility>

namespace klib{
namespace algo {


static std::mt19937 gen;

template<class Iterator, class Comparator>
inline void insertion_sort(const Iterator& beg, const Iterator& end, const Comparator &op)
{
    if(beg != end)
    {
        for(auto i = beg + 1; i != end; ++i)
        {
            auto tmp = *i;
            auto j = i;
            for(; j != beg && op(tmp, *(j - 1)); --j)
                *j = *(j - 1);
            *j = tmp;
        }
    }
}

template<class Iterator>
inline void insertion_sort(const Iterator& beg, const Iterator& end)
{
    typedef typename std::iterator_traits<Iterator>::value_type ElemType;
    insertion_sort(beg, end, std::less<ElemType>());
}

template<class Iterator>
inline void select_pivot(const Iterator& beg, const Iterator& end)
{
    int n = end - beg;
    std::uniform_int_distribution<> dist(0, n - 1);
    auto it = beg + dist(gen);
    if(it != beg)
    {
        std::iter_swap(beg, it);
    }
}

template<class Iterator, class Comparator>
inline Iterator partition(const Iterator& beg, const Iterator& end, const Comparator &op)
{
    auto pivot = *beg;
    auto i = beg;
    auto j = end;
    while(true)
    {
        while(i != end - 1 && op(*(++i), pivot));
        while(op(pivot, *(--j)));
        if(j - i > 0)
            std::iter_swap(i, j);
        else
            break;
    }
    *beg = *j;
    *j = pivot;
    return j;
}

template<class Iterator, class Comparator>
void quick_sort(const Iterator& beg, const Iterator& end, const Comparator &op)
{
    if(end - beg <= 10)
    {
        insertion_sort(beg, end, op);
    }
    else 
    {
        select_pivot(beg, end);
        Iterator center = klib::algo::partition(beg, end, op);
        quick_sort(beg, center, op);
        quick_sort(center + 1, end, op);
    }
}

template<class Iterator>
void quick_sort(const Iterator& beg, const Iterator& end)
{
    typedef typename std::iterator_traits<Iterator>::value_type ElemType;
    quick_sort(beg, end, std::less<ElemType>());
}


}}



#endif


