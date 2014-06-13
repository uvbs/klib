
#include <kthread/multi_value.h>

using namespace klib::kthread;

TEST(multi_int, 1)
{

    safe_uint32 val;
    val += 1;
    EXPECT_EQ((uint32_t)val, 1);

    val += safe_uint32(5);
    EXPECT_EQ((uint32_t)val, 6);

    val -= safe_uint32(3);
    EXPECT_EQ((uint32_t)val, 3);

    val = 0;
    EXPECT_EQ((uint32_t)val, 0);
}