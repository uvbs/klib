/*
  Name: check.h
  Copyright: free head file
  Author: hqwfreefly
  Date: 30-09-10 17:03
  Description: 检查程序中使用malloc申请的内存是否被释放，输出统计结果到_report.txt文件
               如果无法创建_report.txt则输出结果到标准错误输出设备。该头文件的使用方法
               介绍请参考当前目录下的test.c文件。 
               欢迎随时来我的空间做客: www.hi.baidu.com/hqwfreefly 
*/

#ifndef MEMCHECK_
#define MEMCHECK_

#if defined(MEMCHECK)

#include <stdio.h>
#include <stdlib.h>
#define malloc(size) mmalloc(size, __LINE__)
#define free(addr) ffree(addr, __LINE__)
#define __REPORT_FILE "_report.txt"
static void *(*pmalloc)(size_t) = malloc;
static void (*pfree)(void *) = free;
static FILE *___fp = NULL;
#define REPORT show_result()

//初始化，为双向链表的头结点分配空间,打开报告文件 

#define CHECKSTART \
    do \
    { \
        tail = head = (Mem *)pmalloc(sizeof(Mem)); \
        if(head == NULL) \
        { \
            fprintf(stderr, "Fail to init."); \
            exit(1); \
        } \
        head->next = NULL; \
        head->pre = NULL; \
            if((___fp = fopen(__REPORT_FILE, "wt")) == NULL) \
            { \
            fprintf(stderr, "+--------------------------------------------------------+\n"); \
                fprintf(stderr, "|Error: Unable to create _report.txt. Permisson denied. |\a\n"); \
            fprintf(stderr, "+--------------------------------------------------------+\n"); \
            ___fp = stderr; \
            } \
    }while(0)

//结束处理，释放链表,关闭报告文件 

#define CHECKEND \
    do{ \
        Mem *temp0 = head, *temp1 = head->next; \
        for(; temp1 != NULL; temp1 = temp1->next) \
        { \
            pfree(temp0); \
            temp0 = temp1; \
        } \
        fclose(___fp); \
        if(___fp == stderr) \
            getch(); \
    }while(0)

//内存块结构体

typedef struct Mem
{
    size_t size; //大小
    size_t addr; //地址
    size_t line; //申请行号
    int isfreed; //释放标记 1代表已经释放 0代表未释放
    struct Mem *next; //下一内存块指针
    struct Mem *pre; //上一内存块指针
}Mem;

static Mem *head = NULL; //头指针

static Mem *tail = NULL; //尾指针


//申请函数

void *mmalloc(const size_t size, const size_t line)
{
    Mem *new_node = NULL;
    Mem *temp_node = NULL;
    void *addr = NULL;

    addr = pmalloc(size);
    if((new_node = (Mem *)pmalloc(sizeof(Mem))) == 0)
    {
        fprintf(stderr, "Fail to malloc node for recording.");
        exit(1);
    }
    new_node->size = size;
    new_node->addr = (size_t)addr;
    new_node->line = line;
    new_node->isfreed = 0;
    new_node->next = NULL;
    new_node->pre = NULL;
    tail->next = new_node;
    temp_node = tail;
    tail = new_node;
    tail->pre = temp_node;

    return addr;

}

//释放函数

int ffree(void *addr, const size_t line)
{
    size_t temp_addr = (size_t)addr;
    Mem *p = head->next;

    for(; p != NULL; p = p->next)
    {
        if(p->addr == temp_addr)
        {
            p->isfreed = 1;
            pfree(addr);
            return 0;
        }
    }
    fprintf(___fp, "%s%u\n", "Try to free undistributed memory. At line: ", line);
    return -1;
}

//打印信息

void show_result(void)
{
    Mem *temp = head->next;
    size_t total_size = 0;
    size_t freed_size = 0;

    fprintf(___fp, "__________________________________________________________\n");
    fprintf(___fp, "[行号]\t\t[地址]\t\t[大小]\t\t[释放标记]\n");
    for(; temp != NULL; temp = temp->next)
    {
     fprintf(___fp, "%u\t\t0x%x\t%u\t\t%u\n", temp->line, temp->addr, temp->size, temp->isfreed);
     total_size += temp->size;
     freed_size += temp->size * temp->isfreed;
    }
    fprintf(___fp, "__________________________________________________________\n");
    fprintf(___fp, "总计申请: %-37u bytes\n总计释放: %-37u bytes\n内存溢出: %-37u bytes\n",
            total_size, freed_size, total_size - freed_size);
    fprintf(___fp, "__________________________________________________________\n");
    fprintf(___fp, "%s\t%s\n", __DATE__, __TIME__);
    fprintf(___fp, "__________________________________________________________\n");

}

#else
    #define CHECKSTART
    #define CHECKEND
    #define REPORT
#endif

#endif
