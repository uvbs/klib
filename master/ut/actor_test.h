


using namespace klib::pattern::actor; 

class myactor : public actor_imp<myactor, int>
{
public:
    myactor () : m_sum(0) {}

    void execute(int& t) 
    {
        m_sum += t;
    }
    
protected:
    int m_sum;
};



TEST(actor, 1)
{
    engine ngn;
    ngn.init();

    myactor act;
    ngn.regist(&act);
    
    for (int i=0; i< 1; ++i) 
    {
        act.send(i);
    }
    act.sync();
    ngn.stop();

}