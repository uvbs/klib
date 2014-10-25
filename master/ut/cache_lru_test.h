
#include <pattern/cache_lru.h>
using namespace klib::pattern;

#include <string>

TEST(cache_lru_tesst, 1)
{
    typedef cache_lru<int, int>  cache_test_type;

    auto detach_func = [](const int& k, const int& knew, int* old, int* news)
    {
        int x = 1;
        x = 2;
    };

    cache_test_type cache(1000);
    cache.on_detach(detach_func);

    auto miss_func = [](const int& k, int* v, cache_test_type::self_type* c) -> bool
    {
        *v = 111;
        return false;
    };
    cache.on_miss(miss_func);

    for (int index = 0; index<10000; ++ index)
    {
        cache.put(index, index);
    }

    int val;
    EXPECT_FALSE(cache.get(10000, val));
    EXPECT_TRUE(cache.get(9999, val));
    EXPECT_TRUE(cache.get(9001, val));
    EXPECT_TRUE(cache.get(9000, val));
    EXPECT_FALSE(cache.get(8999, val));
    EXPECT_FALSE(cache.get(8998, val));
}


TEST(cache_lru_tesst, 2)
{
    cache_lru<std::string, int> cache(1000);
    for (int index = 0; index<10000; ++ index)
    {
        cache.put(std::to_string((long long)index), index);
    }

    int val;
    EXPECT_FALSE(cache.get(std::to_string((long long)10000), val));
    EXPECT_TRUE(cache.get(std::to_string((long long)9999), val));
    EXPECT_TRUE(cache.get(std::to_string((long long)9001), val));
    EXPECT_TRUE(cache.get(std::to_string((long long)9000), val));
    EXPECT_FALSE(cache.get(std::to_string((long long)8999), val));
    EXPECT_FALSE(cache.get(std::to_string((long long)8998), val));


}