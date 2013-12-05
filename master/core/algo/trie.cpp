#include "trie.h"


namespace klib{
namespace algo{



Trie::Trie()
{
    root = new Trie_node();
}

void Trie::insert(const char* word)
{
    Trie_node *location = root;
    while(*word)
    {
        if(location->next[*word-'a'] == NULL)//不存在则建立
        {
            Trie_node *tmp = new Trie_node();
            location->next[*word-'a'] = tmp;
        }    
        location = location->next[*word-'a']; //每插入一步，相当于有一个新串经过，指针要向下移动
        word++;
    }
    location->isStr = true; //到达尾部,标记一个串
}

Trie::Trie_node* Trie::searchLocation(const char *word)
{
    Trie_node *location = root;
    while(*word && location)
    {
        location = location->next[*word-'a'];
        word++;
    }
    return location;
}

bool Trie::search(const char *word)
{
    Trie_node *location = root;
    while(*word && location)
    {
        location = location->next[*word-'a'];
        word++;
    }
    return (location!=NULL && location->isStr);
}

void Trie::output(Trie_node * location, const char * word)
{
    if(location->isStr)
    {
        cout<<word<<endl;
    }

    for(int index = 0; index < branchNum; index++)
    {
        if(location->next[index] != NULL)
        {
            char * tmp = new char[strlen(word)+2];
            strcpy(tmp,word);
            tmp[strlen(word)] = 'a'+index;
            tmp[strlen(word)+1] = '\0';
            output(location->next[index], tmp);
            delete tmp;
        }
    }
}

void Trie::deleteTrie(Trie_node *root)
{
    for(int i = 0; i < branchNum; i++)
    {
        if(root->next[i] != NULL)
        {
            deleteTrie(root->next[i]);
        }
    }
    delete root;
}


bool Trie::deleteWord(const char* word)  
{  
    Trie_node * current = root;  
    std::stack<Trie_node*> nodes;//用来记录经过的中间结点，供以后自下而上的删除  
    while (*word != '\0' && current != 0)  
    {  
        nodes.push(current);//经过的中间结点压栈  
        current = current->next[*word - 'a'];  
        word++;
    }  

    if (current && current->isStr)  
    {  
        current->isStr = false;//此时current指向该word对应的叶子结点  
        while (nodes.size() != 0)  
        {  
            char c = *(--word);  
            current = nodes.top()->next[c - 'a'];//取得当前处理的结点  

            bool isNotValid = true;  
            for (int i=0;i<26;++i)//判断该结点是否是所有的next为空或只有该word对应字符域不为空  
            {  
                if (current->next[i] != 0)  
                {  
                    isNotValid = false;  
                }  
            }  
            if (current->isStr == 0 && isNotValid)//当一个节点无效，即它只和该word相关，可以删除  
            {  
                delete current;  
            }  
            else//说明当前这个中间结点也被其他的结点所用，不能删除。  
            {  
                break;  
            }  
            nodes.top()->next[c - 'a'] = 0;//把上层的结点的next中指向current结点的指针清0  
            nodes.pop();  
        }  
        return true;  
    }   
    else 
    {  
        return false;  
    }  
} 

//-----测试1------请注释测试2--------
// void main() //简单测试
// {
//     Trie t;
//     t.insert("abcd");         
//     t.insert("abef");
//     char * c = "acg";
//     t.insert(c);
//     t.insert("acf");
// 
// 	char * test = "ac";
// 	Trie_node * location = t.searchLocation(test);
// 	t.output(location,"ac");
// 	     
// }

//-----测试2------请注释测试1--------
//void main() //deleteWord 的简单测试 
//{
//	Trie t;
//	t.insert("abc");
//	t.insert("d");
//	char * c = "dda";
//	t.insert(c);
//	t.insert("da");
//	if(t.search("da"))
//	{
//		printf("find\n");
//	}
//	t.deleteWord("dda");
//	if(!t.search("dda"))
//	{
//		printf("not find dda\n");
//	}
//	t.deleteWord("d");
//	if(!t.search("d"))
//	{
//		printf("not find d\n");
//	}
//	if(t.search("da"))
//	{
//		printf("find\n");
//	}
//}


}}

