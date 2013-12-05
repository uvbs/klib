#include <iostream>
#include <stack>
using namespace std;

namespace klib {
namespace algo {



const int branchNum = 26; //声明常量 


class Trie
{
public:
    struct Trie_node
    {
        bool isStr; //记录此处是否构成一个串。
        Trie_node *next[branchNum];//指向各个子树的指针,下标0-25代表26字符
        Trie_node():isStr(false)
        {
            memset(next,NULL,sizeof(next));
        }
    };

public:
    Trie();

    Trie_node * getRoot(){return root;}
    void insert(const char* word);
    bool search(const char* word);  // 搜索word是否存在
    Trie_node* searchLocation(const char *word); // 返回word位置
    void output(Trie_node *location, const char * word);
    void deleteTrie(Trie_node *root);
    bool deleteWord(const char* word);

private:
    Trie_node* root;
};



}}
