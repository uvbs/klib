
#include <sstream>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/test/minimal.hpp>

#include <RCF/RCF.hpp>
#include <RCF/util/CommandLine.hpp>
#include <RCF/util/PortNumbers.hpp>

class A
{
public:
    A() : a() {}
    A(int a) : a(a) {}
    virtual ~A() {}
    int a;

    template<typename Archive>
    void serialize(Archive &ar, unsigned int)
    {
        ar & a;
    }
};

class B : public A
{
public:
    B() : A(), b() {}
    B(int a, int b) : A(a),  b(b) {}
    int b;

    template<typename Archive>
    void serialize(Archive &ar, unsigned int)
    {
        serializeParent<A>(ar, *this);
        ar & b;
    }

};

class C : public A
{
public:
    C() : A(), b(), c() {}
    C(int a, int b, int c) : A(a),  b(b), c(c) {}
    int b;
    int c;

    template<typename Archive>
    void serialize(Archive &ar, unsigned int)
    {
        serializeParent<A>(ar, *this);
        ar & b;
        ar & c;
    }
};

// setup the runtime to serialize polymorphic data
AUTO_RUN( SF::registerType<B>("B") );
AUTO_RUN( SF::registerType<C>("C") );

#ifdef __MWERKS__
// Compiler bug in Codewarrior make the following necessary (supposed to be done automatically 
// by SF::registerType<>() )
AUTO_RUN( (SF::registerBaseAndDerived<A,B>()) );
AUTO_RUN( (SF::registerBaseAndDerived<A,C>()) );
#endif

// for boost
//BOOST_CLASS_EXPORT_GUID(B, "B")
//BOOST_CLASS_EXPORT_GUID(C, "C")

// gcc < 3.4 doesn't support boost::is_abstract so we have to be explicit
namespace boost {
    template<>
    struct is_abstract<A>
    {
        enum { value = false };
        typedef boost::mpl::false_ type;
    };
}

// interface definitions
RCF_BEGIN(I_X, "X")
    RCF_METHOD_R1(std::string, f1, A*)
    RCF_METHOD_V2(void, f3, const std::string &, A*&)
    RCF_METHOD_R2(bool, f4, A*, A*)
RCF_END(I_X)

RCF_BEGIN(I_Y, "Y")
    RCF_METHOD_R1(std::string, f1, A&)
    RCF_METHOD_R2(bool, f4, A&, A&)
RCF_END(I_Y)

RCF_BEGIN(I_Z, "Z")
    RCF_METHOD_R1(std::string, f1, boost::shared_ptr<A>)
    RCF_METHOD_R1(boost::shared_ptr<A>, f2, const std::string &)
    RCF_METHOD_V2(void, f3, const std::string &, boost::shared_ptr<A> &)
    RCF_METHOD_R2(bool, f4, boost::shared_ptr<A>, boost::shared_ptr<A>)
RCF_END(I_Z)


class X
{
public:
    std::string f1(A *a)
    {
        return typeid(*a).name();
    }

    void f3(const std::string &s, A *&pa)
    {
        static A *pa_ = new A;
        static B *pb_ = new B;
        static C *pc_ = new C;
        if (s == typeid(A).name())
            pa = pa_;
        else if (s == typeid(B).name())
            pa = pb_;
        else if (s == typeid(C).name())
            pa = pc_;
        else 
            throw std::runtime_error("unknown type name: " + s);
    }

    bool f4(A *pa1, A *pa2)
    {
        return pa1 == pa2;
    }
};

class Y
{
public:
    std::string f1(A &a)
    {
        return typeid(a).name();
    }

    bool f4(A &ra1, A &ra2)
    {
        return &ra1 == &ra2;
    }
};

class Z
{
public:
    std::string f1(boost::shared_ptr<A> a)
    {
        return typeid(*a).name();
    }

    boost::shared_ptr<A> f2(const std::string &s)
    {
        if (s == typeid(A).name())
            return boost::shared_ptr<A>(new A);
        else if (s == typeid(B).name())
            return boost::shared_ptr<B>(new B);
        else if (s == typeid(C).name())
            return boost::shared_ptr<C>(new C);
        else 
            throw std::runtime_error("unknown type name: " + s);
    }
    void f3(const std::string &s, boost::shared_ptr<A> &pa)
    {
        if (s == typeid(A).name())
            pa = boost::shared_ptr<A>(new A);
        else if (s == typeid(B).name())
            pa = boost::shared_ptr<B>(new B);
        else if (s == typeid(C).name())
            pa = boost::shared_ptr<C>(new C);
        else 
            throw std::runtime_error("unknown type name: " + s);
    }

    bool f4(boost::shared_ptr<A> pa1, boost::shared_ptr<A> pa2)
    {
        return pa1.get() == pa2.get();
    }
};

int test_main(int argc, char **argv)
{

    util::CommandLineOption<int> port( "port", util::Ports::getNext(), "port number" );
    util::CommandLine::getSingleton().parse(argc, argv);
    
    RCF::RcfServer server(port);
    server.bind<I_X, X>();
    server.bind<I_Y, Y>();
    server.bind<I_Z, Z>();
    server.start();

    std::string s;
    RcfClient<I_X> x("localhost", port);
    RcfClient<I_Y> y("localhost", port);
    RcfClient<I_Z> z("localhost", port);

    {
        boost::shared_ptr<A> pa(new A);
        boost::shared_ptr<A> pb(new B);
        boost::shared_ptr<A> pc(new C);

        s = x.f1(pa.get()); BOOST_CHECK( s == typeid(A).name() );
        s = x.f1(pb.get()); BOOST_CHECK( s == typeid(B).name() );
        s = x.f1(pc.get()); BOOST_CHECK( s == typeid(C).name() );

        s = y.f1(*pa); BOOST_CHECK( s == typeid(A).name() );
        s = y.f1(*pb); BOOST_CHECK( s == typeid(B).name() );
        s = y.f1(*pc); BOOST_CHECK( s == typeid(C).name() );

        s = z.f1(pa); BOOST_CHECK( s == typeid(A).name() );
        s = z.f1(pb); BOOST_CHECK( s == typeid(B).name() );
        s = z.f1(pc); BOOST_CHECK( s == typeid(C).name() );
    }
    {
        boost::shared_ptr<A> spa;
        spa = z.f2( typeid(B).name() ); BOOST_CHECK( dynamic_cast<B *>(spa.get()) != NULL );
        spa = z.f2( typeid(C).name() ); BOOST_CHECK( dynamic_cast<C *>(spa.get()) != NULL );
    }
    {
        A *pa = NULL;
        x.f3(typeid(B).name(), pa); BOOST_CHECK( dynamic_cast<B *>(pa) != NULL );
        x.f3(typeid(C).name(), pa); BOOST_CHECK( dynamic_cast<C *>(pa) != NULL );
    
        boost::shared_ptr<A> spa;
        z.f3(typeid(B).name(), spa); BOOST_CHECK( dynamic_cast<B *>(spa.get()) != NULL );
        z.f3(typeid(C).name(), spa); BOOST_CHECK( dynamic_cast<C *>(spa.get()) != NULL );
    }
    {
        A *pa1 = new A;
        A *pa2 = pa1;
        A *pa3 = new A;
        A *pa4 = new A;

        A &ra1 = *pa1;
        A &ra2 = *pa2;
        A &ra3 = *pa3;
        A &ra4 = *pa4;

        boost::shared_ptr<A> spa1(pa1);
        boost::shared_ptr<A> spa2(spa1);
        boost::shared_ptr<A> spa3(pa3);
        boost::shared_ptr<A> spa4(pa4);

        // need to enable pointer tracking for following tests to pass
        bool same = false;
        same = x.f4(pa1, pa2); //BOOST_CHECK( same );
        same = x.f4(pa3, pa4); //BOOST_CHECK( !same );
        same = y.f4(ra1, ra2); //BOOST_CHECK( same );
        same = y.f4(ra3, ra4); //BOOST_CHECK( !same );
        same = z.f4(spa1, spa2); //BOOST_CHECK( same );
        same = z.f4(spa3, spa4); //BOOST_CHECK( !same );
    }

    return boost::exit_success;
}

