


using namespace klib::pattern::actor; 

class myactor : public actor_imp<myactor, int>
{
public:
    myactor (engine& fr) : actor_imp( fr) {}

    void execute(int& t) 
    {
        printf("%d ......... \n", t);

    }
};



TEST(actor, 1)
{
    engine ngn;
    ngn.init();

    myactor act(ngn);

    for (int i=0; i< 100; ++i) 
    {
        act.send(i);
    }

    Sleep(1000);
}