


using namespace klib::pattern::actor; 

class myactor : public actor_imp<myactor, int>
{
public:
    myactor () {}

    void execute(int& t) 
    {
        printf("%d ......... \n", t);

    }
};



TEST(actor, 1)
{
    engine ngn;
    ngn.init();

    myactor act;
    ngn.regist(&act);

    for (int i=0; i< 1001; ++i) 
    {
        act.send(i);
    }
    act.sync();

    Sleep(4000);

    //
}