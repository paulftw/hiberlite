#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "hiberlite.h"
using namespace hiberlite;

#include <iostream>
#include <cmath>
#include <vector>
#include <map>
#include <limits>
using namespace std;


class B{
    friend class hiberlite::access;
    template<class Archive>
    void hibernate(Archive & ar)
    {
        ar & HIBERLITE_NVP(name);
        ar & HIBERLITE_NVP(weight);
        ar & HIBERLITE_NVP(height);
    }

    public:
        string name;
        double weight;
        int height;
};

class A{
    friend class hiberlite::access;
    template<class Archive>
    void hibernate(Archive & ar)
    {
        ar & HIBERLITE_NVP(seti);
        ar & HIBERLITE_NVP(i);
        ar & HIBERLITE_NVP(s);
        ar & HIBERLITE_NVP(vi);
        ar & HIBERLITE_NVP(v);
        ar & HIBERLITE_NVP(mm);
    }

    public:
        int i;
        string s;
        vector< pair<int,int> > vi;
        set< double > seti;
        vector< bean_ptr<B> > v;
        map<int,vector<int> > mm;
};

class X{
    friend class hiberlite::access;
    template<class Archive>
    void hibernate(Archive & ar)
    {
        ar & HIBERLITE_NVP(seti);
        ar & HIBERLITE_NVP(vvi);
        ar & HIBERLITE_NVP(mm);
    }
    public:
        set< pair<int,double> > seti;
        vector< vector<int> >  vvi;
        map< vector<string>, map<string,string> > mm;
        bool operator==(const X& x){
            return seti==x.seti && vvi==x.vvi && mm==x.mm;
        }
};

HIBERLITE_EXPORT_CLASS(A)
HIBERLITE_EXPORT_CLASS(B)
HIBERLITE_EXPORT_CLASS(X)

TEST_CASE() {
    Database db("catch_test.db");

    CHECK_NOTHROW( db.registerBeanClass<A>() );
    CHECK_NOTHROW( db.registerBeanClass<B>() );

    vector<string> msg=db.checkModel();
    for(size_t ci=0;ci<msg.size();ci++)
        CAPTURE (std::string("model check reported: "+msg[ci]));
    CHECK( msg.size() == 0 );

    CHECK_NOTHROW( db.dropModel() );
    CHECK_NOTHROW( db.createModel() );

    for(unsigned int i=0;i<5;i++){
        A obj;
        obj.i=i;
        char ss[100];
        sprintf(ss,"obj_num_%d",i);
        obj.s=ss;

        B x;
        for(unsigned int i=0;i<5;i++){
            obj.mm[i%3].push_back(i);
            obj.mm[i%3].push_back(2*i);
            obj.vi.push_back(make_pair(i,i*i*i));
            obj.seti.insert(1.0/i);

            string name("01234567890123123123123");
            x.name="name " + name.substr(i,3);
            x.weight=sqrt(i);
            x.height=-10*i;
            bean_ptr<B> ptr=db.copyBean(x);
            if (!ptr.get_object())
                CAPTURE(x.name);
            CHECK( ptr.get_object() );
            obj.v.push_back(ptr);
        }
        while(obj.v.size()){
            obj.v[0].destroy();
            obj.v.erase(obj.v.begin());
        }
        bean_ptr<A> a=db.copyBean(obj);
        if(a.get_id()%10==0)
            CAPTURE(std::string("save obj with name ")+(*a).s);
        CHECK( (a.get_id()%10) != 0 );
    }
}

TEST_CASE() {
    Database db("catch_test.db");
//	we will not create or drop tables, so model initialization
//  is not necessary
//		db.registerBeanClass<A>();
//		db.registerBeanClass<B>();

    Database otherdb("catch_copy.db");
    CHECK_NOTHROW( otherdb.registerBeanClass<A>() );
    CHECK_NOTHROW( otherdb.registerBeanClass<B>() );

    CHECK_NOTHROW( otherdb.dropModel() );
    CHECK_NOTHROW( otherdb.createModel() );

    bean_ptr<A> be=db.loadBean<A>(1);
    CHECK( be.get_object() );
    A obj2=*be;
    bean_ptr<A> a2=otherdb.copyBean(obj2);
    if (!a2.get_object())
        CAPTURE(std::string("load obj with name ") + (*a2).s);
    CHECK( a2.get_object() );
}

TEST_CASE() {
    X x;
    x.vvi.resize(5);
    map< vector<string>, map<string,string> > mm;
    vector<string> randstr;
    unsigned int NS=30;
    for(unsigned int i=0;i<NS;i++)
        randstr.push_back(hiberlite::Transformer::toSQLiteValue(rand()%100+i));

    for(unsigned int i=0;i<100;i++){
        x.seti.insert( make_pair(i*i,1.0/i+sqrt(i)) );
        x.vvi[(i*i)%5].push_back(10003%(i+1));

        vector<string> vs;
        for(unsigned int i=0;i<5;i++)
            vs.push_back( randstr[(17+7*i)%NS] );
        for(unsigned int i=0;i<10;i++)
            x.mm[vs][ randstr[(19*i)%NS] ]=randstr[(23*i)%NS];
    }

    {
        Database db("catch_t3.db");
        CHECK_NOTHROW( db.registerBeanClass<X>() );
        CHECK_NOTHROW( db.dropModel() );
        CHECK_NOTHROW( db.createModel() );
        CHECK_NOTHROW( db.copyBean(x) );
    }

    {
        Database db("catch_t3.db");
        CHECK_NOTHROW( db.registerBeanClass<X>() );
        bean_ptr<X> xptr=db.loadBean<X>(1);
        CAPTURE( "load failed" );
        CHECK( (*xptr)==x );
    }

}

struct ULL {
    unsigned long long ull;

    template<class Archive>
    void hibernate(Archive & ar)
    {
        ar & HIBERLITE_NVP(ull);
    }
};

HIBERLITE_EXPORT_CLASS(ULL);

TEST_CASE("unsigned long long should survive serialization") {
    hiberlite::Database db("ull.db");
    db.registerBeanClass<ULL>();
    db.dropModel();
    db.createModel();

    ULL max = { numeric_limits<unsigned long long>::max() };
    // struct to avoid compiler warning C4307
    REQUIRE((max.ull + 1) == 0);
    bean_ptr<ULL> max_old = db.copyBean(max);
    bean_ptr<ULL> max_new = db.loadBean<ULL>(max_old.get_id());
    CHECK(max_new->ull == max.ull);
}


enum class Gender {
    male, female
};

struct Person {

    Gender gender_;

    template <class Archive>
    void hibernate(Archive & ar) {
        ar & HIBERLITE_NVP(gender_);
    }
};

namespace hiberlite {
    template<class A> void hibernate(A& ar, Gender& value, const unsigned int)
    {
        ar & db_atom<Gender>(value);
    }

    template<> inline std::string db_atom<Gender>::sqliteStorageClass()
    {
        return "INTEGER";
    }

    template<> template<class Stmt, class Arg> void db_atom<Gender>::loadValue(Stmt& res, Arg& arg)
    {
        val = static_cast<Gender>(res.get_int(arg));
    }

    template<> inline void db_atom<Gender>::bindValue(sqlite3_stmt* stmt, int col)
    {
        sqlite3_bind_int(stmt, col, static_cast<int>(val));
    }
}

HIBERLITE_EXPORT_CLASS(Person);

SCENARIO("custom enum type conversions") {
    GIVEN("a db") {
        hiberlite::Database db("custom.db");
        {
            WHEN("I add register a type with an enum field") {
                db.registerBeanClass<Person>();
                db.dropModel();
                db.createModel();

                AND_WHEN("I save some entries in the db") {
                    Person m = { Gender::male };
                    Person f = { Gender::female };

                    db.copyBean(m);
                    db.copyBean(f);

                    THEN("The db contains the entries with the correct enum values") {
                        auto ppl = db.getAllBeans<Person>();
                        CHECK(ppl.size() == 2);
                        CHECK(ppl[0]->gender_ == Gender::male);
                        CHECK(ppl[1]->gender_ == Gender::female);
                    }
                }
            }
        }
    }
}
