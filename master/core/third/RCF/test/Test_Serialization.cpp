
// disable ADL warning for vc71
#ifdef _MSC_VER
#pragma warning( disable : 4675 ) // warning C4675: '...' : resolved overload was found by argument-dependent lookup
#endif

#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include <boost/test/minimal.hpp>
#include <boost/test/floating_point_comparison.hpp>
#define BOOST_CHECK_CLOSE(d1, d2, tol) BOOST_CHECK( boost::test_tools::check_is_close(d1, d2, tol) )
//#define BOOST_CHECK_CLOSE(d1, d2, tol)
//#define BOOST_CHECK(x) if (x) ; else { std::cout << "FAIL: " << #x << "\n"; }
//namespace boost { static const int exit_success = 0; }

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

// streams
#include <SF/IBinaryStream.hpp>
#include <SF/OBinaryStream.hpp>
#include <SF/INativeBinaryStream.hpp>
#include <SF/ONativeBinaryStream.hpp>
#include <SF/ITextStream.hpp>
#include <SF/OTextStream.hpp>

// construction and registation
#include <SF/SfNew.hpp>
#include <SF/Registry.hpp>

// serializers
#include <SF/list.hpp>
#include <SF/map.hpp>
#include <SF/memory.hpp>
#include <SF/scoped_ptr.hpp>
#include <SF/set.hpp>
#include <SF/shared_ptr.hpp>
#include <SF/string.hpp>
#include <SF/vector.hpp>

#include <SF/SerializeEnum.hpp>

//**************************************************

double fpt_tolerance = .1;

bool fpt_compare(double *pd1, double *pd2, unsigned int count)
{
    for (unsigned int i=0; i<count; i++)
        if ((pd1[i]-pd2[i])*(pd1[i]-pd2[i]) > fpt_tolerance)
            return false;
    return true;
}

bool fpt_compare(float *pf1, float *pf2, unsigned int count)
{
    for (unsigned int i=0; i<count; i++)
        if ((pf1[i]-pf2[i])*(pf1[i]-pf2[i]) > fpt_tolerance)
            return false;
    return true;
}

namespace Test {

    struct GeomObject
    {
        GeomObject() : x(0), y(0) {}
        GeomObject(int x, int y) : x(x), y(y) {}
        virtual ~GeomObject() {}
        bool operator==(const GeomObject &rhs) { return x == rhs.x && y == rhs.y; }
        int x;
        int y;
    };

    struct Rect : public GeomObject
    {
        Rect() : GeomObject(), w(0), h(0) {}
        Rect(int x,int y, int w, int h) : GeomObject(x,y), w(w), h(h) {}
        bool operator==(const Rect &rhs) { return x == rhs.x && y == rhs.y && w == rhs.w && h == rhs.h; }
        int w;
        int h;
    };

    class Empty {
    public:
        Empty() {}
    };

    struct D { virtual ~D() {} };
    struct A;
    struct B;
    struct C;

    struct A : public D { B *pb; };
    struct B : public D { C *pc; };
    struct C : public D { A *pa; };

    template<typename T>
    struct TestOne
    {
        bool operator==(const TestOne &rhs) { return n == rhs.n && t == rhs.t; }
        int n;
        T t;
    };

    template<typename T, typename U>
    struct TestTwo
    {
        bool operator==(const TestTwo &rhs) { return t == rhs.t && u == rhs.u; }
        T t;
        U u;
    };

    enum TestEnum { MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY, SATURDAY, SUNDAY };

    class ArrayContainer {
    public:
        ArrayContainer() 
        { 
            memset(this, 0, sizeof(ArrayContainer)); 
        }
        ArrayContainer(int n) { 
            memset(this, 0, sizeof(ArrayContainer));
            if (n) {

                srand(1); // Reinitialize random number generator so we get the same sequence each time

                for (int i=0; i<NSIZE; i++)
                    pn[i] = rand();

                for (int i=0; i<CHSIZE; i++)
                    for (int j=0; j<CHSIZE; j++)
                        for (int k=0; k<CHSIZE; k++)
                            ch[i][j][k] = 'A' + i + j + k;

                pd = new double[DSIZE];
                pd_length = DSIZE;
                double pi = 3.141592654E-10;
                for (int i=0; i<DSIZE; i++) 
                    pd[i] = rand()*pi;

                for (int i=0; i<PIISIZE; i++)
                    for (int j=0; j<PIISIZE; j++)
                        pii[i][j] = std::pair<int,int>(i,j);

                pcc = new std::pair<char,char>[PCCSIZE];
                pcc_length = PCCSIZE;
                for (int i=0; i<PIISIZE; i++)
                    pcc[i] = std::pair<char,char>('A'+i,'A'+i);

            }
        }
        ~ArrayContainer()
        {
            if (pd) delete [] pd;
            if (pcc) delete [] pcc;
        }

        friend bool operator==( const ArrayContainer &lhs, const ArrayContainer &rhs )
        {
            bool bOk = true;
            bOk = bOk && (lhs.pn && rhs.pn && 0 == memcmp(&lhs.pn, &rhs.pn, ArrayContainer::NSIZE*sizeof(int)));
            bOk = bOk && (lhs.ch && rhs.ch && 0 == memcmp(&lhs.ch, &rhs.ch, ArrayContainer::CHSIZE*ArrayContainer::CHSIZE*sizeof(char)));
            bOk = bOk && (lhs.pd_length == rhs.pd_length) && (fpt_compare(lhs.pd, rhs.pd, lhs.pd_length));
            bOk = bOk && (lhs.pii && rhs.pii && 0 == memcmp(&lhs.pii, &rhs.pii, ArrayContainer::PIISIZE*ArrayContainer::PIISIZE*sizeof(std::pair<int,int>)));
            bOk = bOk && (lhs.pcc_length == rhs.pcc_length) && (0 == memcmp(lhs.pcc, rhs.pcc, sizeof(std::pair<char,char>)*lhs.pcc_length) );
            return bOk;
        }

    public:

        static const int NSIZE = 100;
        static const int CHSIZE = 3;
        static const int DSIZE = 100;
        static const int PIISIZE = 4;
        static const int PCCSIZE = 4;

        int pn[NSIZE];
        char ch[CHSIZE][CHSIZE][CHSIZE];
        double *pd;
        unsigned int pd_length;

        std::pair<int,int> pii[PIISIZE][PIISIZE];
        std::pair<char,char> *pcc;
        unsigned int pcc_length;
    };

    class Props {
    public:
        Props() : width_(0) {}
        int get_Width() { return width_; }
        void set_Width(int width) { width_ = width; }
        friend bool operator==( const Props &lhs, const Props &rhs ) { return lhs.width_ == rhs.width_; }
    private:
        int width_;
    };

    namespace K1 {

        class A {
        public:
            A() : a(0) {}
            A(int a) : a(a) {}
            friend bool operator==(const A &lhs, const A &rhs) { return lhs.a == rhs.a; }

            template<typename Archive>
            void serialize(Archive &ar, const unsigned int)
            {
                ar & a;
            }
        private:
            int a;
        };

        namespace K2 {

            class B {
            public:
                B() : b(0) {}
                B(int b) : b(b) {}
                friend bool operator==(const B &lhs, const B &rhs) { return lhs.b == rhs.b; }

                template<typename Archive>
                void serialize(Archive &ar, const unsigned int)
                {
                    ar & b;
                }
            private:
                int b;
            };


        } //namespace K2

    } //namespace K1

} // namespace Test


namespace Test {

    template<typename Archive>
    void serialize(Archive &ar, D &d, const unsigned int)
    {}

    template<typename Archive>
    void serialize(Archive &ar, A &a, const unsigned int)
    {
        ar & a.pb;
    }

    template<typename Archive>
    void serialize(Archive &ar, B &b, const unsigned int)
    {
        ar & b.pc;
    }

    template<typename Archive>
    void serialize(Archive &ar, C &c, const unsigned int)
    {
        ar & c.pa;
    }

    AUTO_RUN( SF::registerType<A>("Test::A") );
    AUTO_RUN( SF::registerType<B>("Test::B") );
    AUTO_RUN( SF::registerType<C>("Test::C") );

    // following necessary since A, B, C serializers don't invoke D's serializer
    AUTO_RUN( (SF::registerBaseAndDerived<D, A>()) );
    AUTO_RUN( (SF::registerBaseAndDerived<D, B>()) );
    AUTO_RUN( (SF::registerBaseAndDerived<D, C>()) );

    template<typename Archive>
    void serialize(Archive &ar, GeomObject &geomObject, const unsigned int)
    {
        ar & geomObject.x & geomObject.y;
    }

    template<typename Archive>
    void serialize(Archive &ar, Rect &rect, const unsigned int)
    {
        serializeParent<GeomObject>(ar, rect);
        ar & rect.w & rect.h;
    }

    AUTO_RUN( SF::registerType<Rect>("Test::Rect") );

    template<typename Archive>
    void serialize(Archive &ar, Empty &e, const unsigned int)
    {}

    template<typename Archive, typename T>
    void serialize(Archive &ar, TestOne<T> &t, const unsigned int)
    {
        ar & t.t & t.n;
    }

    template<typename Archive, typename T, typename U>
    void serialize(Archive &ar, TestTwo<T,U> &t, const unsigned int)
    {
        ar & t.t & t.u;
    }

    template<typename Archive>
    void serialize(Archive &ar, TestOne<int> &t, const unsigned int)
    {
        ar & t.t & t.n;
    }

    // TODO: put this in a macro (SF_SERIALIZE_ENUM or something)
    SF_SERIALIZE_ENUM(TestEnum)
        /*
    template<typename Archive>
    void serialize(Archive &ar, TestEnum &t, const unsigned int)
    {
        // TODO
        if (ar.isRead())
        {
            int n = 0;
            ar & n;
            t = TestEnum(n);
        }
        else // if (ar.isWrite()))
        {
            int n = t;
            ar & n;
        }
    }*/

    template<typename Archive>
    void serialize(Archive &ar, Props &p, const unsigned int)
    {
        if (ar.isRead())
        {
            int width;
            ar & width;
            p.set_Width(width);
        }
        else // if (ar.isWrite())
        {
            ar & p.get_Width();
        }
    }


#if defined(__BORLANDC__)
    // TODO: sort out array serialization with borland
    /*
    // Implicit type detection of inline static arrays is broken in BCC 5.5, so need to specify the types explicitly
    typedef std::pair<int,int> pair_int_int_t;
    SF_BEGIN( ArrayContainer, "" )
        SF_TYPED_MEMBER( pn_, "pn", int [ArrayContainer::NSIZE] );
        SF_TYPED_MEMBER( ch_, "ch", char [ArrayContainer::CHSIZE][ArrayContainer::CHSIZE][ArrayContainer::CHSIZE] );
        SF_DYNAMIC_ARRAY(pd, SF_THIS->pd_length, "pd");
        SF_TYPED_MEMBER( pii_, "pii", pair_int_int_t [ArrayContainer::PIISIZE][ArrayContainer::PIISIZE] );
        SF_DYNAMIC_ARRAY(pcc, SF_THIS->pcc_length, "pcc");
    SF_END( ArrayContainer );
    */

#else

    template<typename Archive>
    void serialize(Archive &ar, ArrayContainer &a, const unsigned int)
    {
        ar & a.pn;
        ar & a.ch;
        ar & SF::dynamicArray(a.pd, a.pd_length);
        ar & a.pii;
        ar & SF::dynamicArray(a.pcc, a.pcc_length);
    }

#endif


} // namespace Test



//********************************************************************
template<typename IStream, typename OStream>
void testAtomic()
{
    using namespace Test;

    bool b1 = true, b2 = false;
    int n1 = 1, n2 = 0;
    char ch1 = 'A', ch2 = 'B';
    float f1 = 2.71828f, f2 = 0.0f;
    double d1 = -3.141592654E-8, d2 = 0.0;

    std::ostringstream os;
    OStream ostr(os);
    ostr << b1 << n1 << ch1 << f1 << d1;

    std::istringstream is(os.str());
    IStream istr(is);
    istr >> b2 >> n2 >> ch2 >> f2 >> d2;

    BOOST_CHECK( b2 == b1 );
    BOOST_CHECK( n2 == n1 );
    BOOST_CHECK( ch2 == ch1 );
    BOOST_CHECK_CLOSE(f2, f1, 1.1 );
    BOOST_CHECK_CLOSE(d2, d1, 1.1 );
}

template<typename IStream, typename OStream>
void testComposite()
{
    using namespace Test;

    GeomObject geom1(1,2), geom2(0,0);
    Rect rect1(1,2,3,4), rect2(0,0,0,0);

    TestOne<bool> to1, to2;
    to1.t = true; to1.n = 1;
    to2.t = false; to2.n = 0;

    TestTwo<std::string, double> tt1, tt2;
    tt1.t = "pi"; tt1.u = 3.14;
    tt2.t = ""; tt2.u = 0;

    std::pair<char, int> p1('a', 1), p2('\0', 0);

    Props prop1, prop2;
    prop1.set_Width(1);
    prop2.set_Width(0);

    std::ostringstream os;
    OStream ostr(os);
    ostr << geom1 << rect1 << to1 << tt1 << p1 << prop1;

    std::istringstream is(os.str());
    IStream istr(is);
    istr >> geom2 >> rect2 >> to2 >> tt2 >> p2 >> prop2;

    BOOST_CHECK( geom2 == geom1 );
    BOOST_CHECK( rect2 == rect1 );
    BOOST_CHECK( to2 == to1 );
    BOOST_CHECK( tt2 == tt1 );
    BOOST_CHECK( p2 == p1 );
    BOOST_CHECK( prop2 == prop1 );

}

template<typename IStream, typename OStream>
void testEmptyComposite()
{

    using namespace Test;

    Empty e1, e2;
    Empty *pe1 = NULL, *pe2 = &e1, *pe3 = NULL, *pe4 = NULL;
    std::vector<int> v1, v2;
    std::vector<int> *pv1 = NULL, *pv2 = &v1, *pv3 = NULL, *pv4 = NULL;

    std::ostringstream os;
    OStream ostr(os);
    ostr << e1 << pe1 << pe2;
    ostr << v1 << pv1 << pv2;

    std::istringstream is(os.str());
    IStream istr(is);
    istr >> e2 >> pe3 >> pe4;
    istr >> v2 >> pv3 >> pv4;

    BOOST_CHECK( pe3 == NULL );
    BOOST_CHECK( pe4 != NULL );
    BOOST_CHECK( v2.empty() );
    BOOST_CHECK( pv3 == NULL );
    BOOST_CHECK( pv4 != NULL );

    //if (pe3) delete pe3;
    //if (pe4) delete pe4;
    //if (pv3) delete pv3;
    //if (pv4) delete pv4;
}


template<typename IStream, typename OStream>
void testArrays()
{
    using namespace Test;

    const SF::UInt32 SIZE = 40;

    // Single dimensional static array
    int pn1[SIZE];
    for (int i=0;i<SIZE;i++)
        pn1[i] = i-20;

    int pn2[SIZE];
    memset(pn2, 0, SIZE*sizeof(int));

    // Single dimensional dynamic array
    int *pn3 = new int[SIZE];
    memcpy(pn3, pn1, SIZE*sizeof(int));

    const int *pn4 = NULL;
    SF::UInt32 n4 = 0;

    // Multidimensional static array
    int pn5[5][5][5];
    for (int i=0; i<5; i++)
        for (int j=0; j<5; j++)
            for (int k=0;k<5; k++)
                pn5[i][j][k] = i*j*k;

    int pn6[5][5][5];
    memset(pn6, 0, 5*5*5*sizeof(int));

    const int pn7[2][2] = { {1,2}, {3,4} };
    const int pn8[2][2] = { {0,0}, {0,0} };

    // Object containing arrays
    ArrayContainer a1(1), a2(0);

#ifndef __BORLANDC__
    {
        // Serialization of multidimensional arrays doesnt work with BCB, except as members of a class,
        // due to compiler bugs.
        std::ostringstream os;
        OStream ostr(os);
        ostr << pn5;
        ostr << pn7;

        std::istringstream is(os.str());
        IStream istr(is);
        istr >> pn6;
        istr >> pn8;

        BOOST_CHECK( pn5 && pn6 && 0 == memcmp( pn5, pn6, 5*5*5*sizeof(int) ) );
        BOOST_CHECK( pn7 && pn8 && 0 == memcmp( pn7, pn8, 2*2*sizeof(int) ) );
    }
#endif

    std::ostringstream os;
    OStream ostr(os);
    ostr << pn1;
    ostr << SF::dynamicArray(pn3,SIZE);
    ostr << a1;

    std::istringstream is(os.str());
    IStream istr(is);
    istr >> pn2;
    istr >> SF::dynamicArray(pn4, n4);
    istr >> a2;

    BOOST_CHECK( pn1 && pn2 && 0 == memcmp( pn1, pn2, SIZE*sizeof(int) ) );
    BOOST_CHECK( pn3 && pn4 && 0 == memcmp( pn3, pn4, SIZE*sizeof(int) ) );
    BOOST_CHECK( a1 == a2 );

    delete [] pn3;
    delete [] pn4;
}


template<typename IStream, typename OStream>
void testEnum()
{
    using namespace Test;

    TestEnum x1,y1,z1, x2,y2,z2;
    x1 = MONDAY;
    y1 = TUESDAY;
    z1 = SUNDAY;
    x2 = SUNDAY;
    y2 = SUNDAY;
    z2 = SUNDAY;

    std::ostringstream os;
    OStream ostr(os);
    ostr << x1 << y1 << z1;

    std::istringstream is(os.str());
    IStream istr(is);
    istr >> x2 >> y2 >> z2;

    BOOST_CHECK( x1 == x2 );
    BOOST_CHECK( y1 == y2 );
    BOOST_CHECK( z1 == z2 );
}


template<typename IStream, typename OStream>
void testContainers()
{
    using namespace Test;

    std::list<int> L1,L2;
    std::set<double> S1,S2;
    std::map<std::string, bool> M1,M2;
    std::vector< std::pair<int,int> > V1,V2;

    L1.push_back(1);
    L1.push_back(2);
    L1.push_back(3);

    S1.insert(1.1);
    S1.insert(2.2);
    S1.insert(3.3);

    M1["one"] = true;
    M1["two"] = false;
    M1["three"] = true;

    std::string str1 = "In the beginning", str2;

    std::ostringstream os;
    OStream ostr(os);
    ostr << L1;
    ostr << S1;
    ostr << M1;
    ostr << V1;
    ostr << str1;

    std::istringstream is(os.str());
    IStream istr(is);
    istr >> L2;
    istr >> S2;
    istr >> M2;
    istr >> V2;
    istr >> str2;

    BOOST_CHECK( L2.size() == L1.size() );
    BOOST_CHECK( std::equal(L1.begin(), L1.end(), L2.begin()) );
    BOOST_CHECK( S2.size() == S1.size() );
    BOOST_CHECK( std::equal(S1.begin(), S1.end(), S2.begin()) );
    BOOST_CHECK( M2.size() == M1.size() );
    BOOST_CHECK( std::equal(M1.begin(), M1.end(), M2.begin()) );
    BOOST_CHECK( V2.size() == V1.size() );
    BOOST_CHECK( std::equal(V1.begin(), V1.end(), V2.begin()) );
    BOOST_CHECK( str2.size() == str1.size() );
    BOOST_CHECK( str1 == str2 );
}

template<typename IStream, typename OStream>
void testGraphs()
{
    using namespace Test;

    A a1,a2;
    B b1,b2;
    C c1,c2;
    D d1,d2;

    int n = 17;
    int *pn = &n;

    int m = 0;
    int *pm = NULL;

    std::ostringstream os;
    OStream ostr(os);
    ostr.enableContext();

    {
        a1.pb = &b1;
        b1.pc = &c1;
        c1.pa = &a1;
        std::vector<D *> vec;
        vec.push_back(&a1);
        vec.push_back(&b1);
        vec.push_back(&c1);
        vec.push_back(&a1);
        vec.push_back(&b1);
        vec.push_back(&c1);

        ostr << &n << pn;
        ostr << d1;
        ostr << vec;
        ostr.clearContext();
        ostr << a1;
        ostr.clearContext();

        a1.pb = NULL;
        b1.pc = NULL;
        c1.pa = NULL;
        vec.clear();
        vec.push_back(&a1);
        vec.push_back(&b1);
        vec.push_back(&c1);
        vec.push_back(&a1);
        vec.push_back(&b1);
        vec.push_back(&c1);

        ostr << vec;
        ostr.clearContext();
        ostr << a1 ;
        ostr.clearContext();
    }

    std::istringstream is(os.str());
    IStream istr(is);
    {
        a2.pb = NULL;
        b2.pc = NULL;
        c2.pa = NULL;
        std::vector<D *> vec;

        istr >> m >> pm;
        istr >> d2;
        istr >> vec;
        istr >> a2 ;

        BOOST_CHECK( m == n);
        BOOST_CHECK( pm == &m );
        BOOST_CHECK( vec.size() == 6 );
        BOOST_CHECK( vec.at(0) == vec.at(3) );
        BOOST_CHECK( vec.at(1) == vec.at(4) );
        BOOST_CHECK( vec.at(2) == vec.at(5) );
        BOOST_CHECK( ((A *) vec.at(0))->pb == ((B *) vec.at(1)) );
        BOOST_CHECK( ((B *) vec.at(1))->pc == ((C *) vec.at(2)) );
        BOOST_CHECK( ((C *) vec.at(2))->pa == ((A *) vec.at(0)) );

        BOOST_CHECK( a2.pb->pc->pa == &a2 );

        delete vec.at(0);
        delete vec.at(1);
        delete vec.at(2);
        vec.clear();

        delete a2.pb->pc;
        delete a2.pb;

        istr >> vec;
        istr >> a2 ;

        BOOST_CHECK( vec.size() == 6 );
        BOOST_CHECK( vec.at(0) == vec.at(3) );
        BOOST_CHECK( vec.at(1) == vec.at(4) );
        BOOST_CHECK( vec.at(2) == vec.at(5) );
        BOOST_CHECK( ((A *) vec.at(0))->pb == NULL );
        BOOST_CHECK( ((B *) vec.at(1))->pc == NULL );
        BOOST_CHECK( ((C *) vec.at(2))->pa == NULL );

        BOOST_CHECK( a2.pb == NULL );

        delete vec.at(0);
        delete vec.at(1);
        delete vec.at(2);
    }
}


template<typename IStream, typename OStream>
void testPolymorphism()
{
    using namespace Test;

    Rect rect(1,2,3,4);
    GeomObject geom(1,2);
    GeomObject *pg1 = NULL, *pg2 = NULL, *pg3 = NULL;
    Rect *pr1 = NULL, *pr2 = NULL;
    GeomObject &rg1 = geom, &rg2 = rect;

    GeomObject g;
    Rect r;
    GeomObject &rg3 = g;
    GeomObject &rg4 = r;

    pg1 = NULL;
    pg2 = (GeomObject *) &geom;
    pg3 = (GeomObject *) &rect;
    pr1 = NULL;
    pr2 = &rect;

    std::ostringstream os;
    OStream ostr(os);
    ostr << pg1 << pg2 << pg3;
    ostr << pr1 << pr2;
    ostr << rg1 << rg2;

    pg1 = reinterpret_cast<GeomObject *>(1);
    pg2 = reinterpret_cast<GeomObject *>(1);
    pg3 = reinterpret_cast<GeomObject *>(1);
    pr1 = reinterpret_cast<Rect *>(1);
    pr2 = reinterpret_cast<Rect *>(1);

    std::istringstream is(os.str());
    IStream istr(is);
    istr >> pg1 >> pg2 >> pg3;
    istr >> pr1 >> pr2;
    istr >> rg3 >> rg4;

    BOOST_CHECK( pg1 == NULL );
    BOOST_CHECK( *pg2 == geom );
    BOOST_CHECK( dynamic_cast<Rect*>(pg3) != NULL && * (dynamic_cast<Rect*>(pg3)) == rect );
    BOOST_CHECK( pr1 == NULL );
    BOOST_CHECK( *pr2 == rect );
    BOOST_CHECK( rg1 == rg3 );
    BOOST_CHECK( *(static_cast<Rect *>(&rg2)) == *(static_cast<Rect *>(&rg4)) );

    delete pg2;
    delete pg3;
    delete pr2;
}

template<typename IStream, typename OStream>
void testSmartPointers()
{
    using namespace Test;

    {
        // std::auto_ptr

        int *pn = new int;
        *pn = 1;
        std::auto_ptr<int> apn(pn);
        std::auto_ptr<double> apd( (double *) NULL );

        std::ostringstream os;
        OStream ostr(os);
        ostr.enableContext();
        ostr << pn << apn << apd;

        pn = NULL;
        apn.reset();
        
        std::istringstream is(os.str());
        IStream istr(is);
        istr >> pn >> apn >> apd;

        BOOST_CHECK( pn != NULL && pn == apn.get() );
        BOOST_CHECK( apd.get() == NULL );
    }

    {

        // boost::shared_ptr, boost::scoped_ptr

        int *pn = new int;
        *pn = 2;
        boost::shared_ptr<int> spn1(pn);
        boost::shared_ptr<int> spn2 = spn1;
        boost::shared_ptr<int> spn3 = spn2;
        boost::shared_ptr<double> spd( (double *) NULL );
        boost::scoped_ptr<int> scpn1( new int(53) );
        boost::scoped_ptr<int> scpn2;

        std::ostringstream os;
        OStream ostr(os);
        ostr.enableContext();
        ostr << pn;
        ostr << spn1;
        ostr << spn2;
        ostr << spn3;
        ostr << spd;
        ostr << scpn1;

        pn = NULL;
        spn1.reset();
        spn2.reset();
        spn3.reset();

        std::istringstream is(os.str());
        IStream istr(is);
        istr >> pn >> spn1 >> spn2 >> spn3 >> spd >> scpn2;

        BOOST_CHECK( pn != NULL );
        BOOST_CHECK( pn == spn1.get() && pn == spn2.get() && pn == spn3.get() );
        BOOST_CHECK( spn1.use_count() == 3 );
        BOOST_CHECK( spd.get() == NULL );
        BOOST_CHECK( scpn1.get() && scpn2.get() && *scpn1 == *scpn2 );
    }
}

template<typename IStream, typename OStream>
void testADL()
{
    // argument dependent lookup

    Test::K1::A a1(1),a2(0);
    Test::K1::K2::B b1(1),b2(0);

    std::ostringstream os;
    OStream ostr(os);
    ostr << a1 << b1;

    std::istringstream is(os.str());
    IStream istr(is);
    istr >> a2 >> b2;

    BOOST_CHECK( a1 == a2 );
    BOOST_CHECK( b1 == b2 );
}

template<typename IStream, typename OStream>
void testAll()
{
    testAtomic<IStream,OStream>();
    testComposite<IStream,OStream>();
    testEmptyComposite<IStream,OStream>();
#ifndef __BORLANDC__
    testArrays<IStream,OStream>();
#endif
    testEnum<IStream,OStream>();
    testContainers<IStream,OStream>();
    testGraphs<IStream,OStream>();
    testPolymorphism<IStream,OStream>();
    testSmartPointers<IStream,OStream>();
    testADL<IStream,OStream>();
}

//***********************************************************

int test_main(int argc, char **argv)
{
    testAll<SF::ITextStream, SF::OTextStream>();
    testAll<SF::IBinaryStream, SF::OBinaryStream>();
    testAll<SF::INativeBinaryStream, SF::ONativeBinaryStream>();
    return boost::exit_success;
}
