#pragma once


#include "pattern/object_reuser.h"


TEST(object_reuser, create)
{
    class CTestObject
    {
    public:
        INT  val;
    };

    class CTestObject2
    {
    public:
        CTestObject2(int param) {}

    public:
        INT val;
    };


    klib::pattern::object_reuser<CTestObject> objectReuse;
    CTestObject* pObj = objectReuse.create_object();
    ASSERT_TRUE(pObj != NULL);
    objectReuse.recyle_object(pObj);


    klib::pattern::object_reuser_sync<CTestObject2> object_sync;
    CTestObject2* pObj2 = object_sync.create_object(1);
    ASSERT_TRUE(pObj2 != NULL);
    object_sync.recyle_object(pObj2);
}