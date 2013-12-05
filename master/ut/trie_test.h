#pragma once

#include <algo/trie.h>

TEST(Trie, 1)
{
    Trie t;
    t.insert("abcd");         
    t.insert("abef");
    char * c = "acg";
    t.insert(c);
    t.insert("acf");

    char * test = "ac";
    Trie::Trie_node * location = t.searchLocation(test);
    t.output(location,"ac");

}