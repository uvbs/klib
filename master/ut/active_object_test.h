

#include <pattern/active_object.h>

class myobject : public klib::pattern::active_object<INT>
{
public:
    bool execute(INT& t) 
    {
        printf("hello \n");
        return true; 
    }
};


TEST(active_object, 1)
{


    myobject ac_;
    ac_.start();
    ac_.send(1);
    ac_.send(1);
    
    while (ac_.size() > 0)
        Sleep(100);

    ac_.stop();


}