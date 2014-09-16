
#include <algo/search/sunday.h>


TEST(sunday, 1)
{
    klib::algo::sunday sd;
    const char* str_pattn = "abcdef";

    sd.set_pattern(str_pattn, str_pattn + 6);

    const char* str_text = "123456789abcdeflssss";
    size_t text_len = strlen(str_text);

    int pos1 = sd.search(str_text, str_text + text_len);
    int pos2 = sd.search(str_text, text_len);
    EXPECT_EQ(pos1, pos2);
    EXPECT_EQ(pos1, 9);
}