
#include <algo/search/sunday.h>


TEST(sunday, 1)
{
    klib::algo::sunday sd;
    const char* str_pattn = "abcdef";

    sd.set_pattern(str_pattn, str_pattn + 6);

    const char* str_text = "123456789abcdeflssss";
    int pos = sd.search(str_text, str_text + strlen(str_text));

    pos = 1;
}