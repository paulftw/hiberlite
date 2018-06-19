
#include "hiberlite.h"
using namespace hiberlite;

#include <iostream>
#include <cmath>
#include <vector>
#include <map>
#include <stdlib.h>
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

class WrappedBytes {
    friend class hiberlite::access;
    template<class Archive>
	void hibernate(Archive & ar)
	{
  	  ar & HIBERLITE_NVP(bytes);
	}
	public:
		vector<uint8_t> bytes;
		bool operator==(const WrappedBytes& wrappedBytes) {
			return bytes == wrappedBytes.bytes;
		}
};

HIBERLITE_EXPORT_CLASS(A)
HIBERLITE_EXPORT_CLASS(B)
HIBERLITE_EXPORT_CLASS(X)
HIBERLITE_EXPORT_CLASS(WrappedBytes)

struct Tester{
	void test1(){
		Database db("test.db");

		db.registerBeanClass<A>();
		db.registerBeanClass<B>();

		vector<string> msg=db.checkModel();
		for(size_t ci=0;ci<msg.size();ci++)
			cout << "model check reported: " << msg[ci] << endl;

		db.dropModel();
		db.createModel();

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
				obj.v.push_back(ptr);
			}
			while(obj.v.size()){
				obj.v[0].destroy();
				obj.v.erase(obj.v.begin());
			}
			bean_ptr<A> a=db.copyBean(obj);
			if(a.get_id()%10==0)
				cout << "save obj with name " << (*a).s << endl;
		}
	}
	void test2(){
		Database db("test.db");
//	we will not create or drop tables, so model initialization
//  is not necessary
//		db.registerBeanClass<A>();
//		db.registerBeanClass<B>();

		Database otherdb("copy.db");
		otherdb.registerBeanClass<A>();
		otherdb.registerBeanClass<B>();

		otherdb.dropModel();
		otherdb.createModel();

		bean_ptr<A> be=db.loadBean<A>(1);
		A obj2=*be;
		bean_ptr<A> a2=otherdb.copyBean(obj2);
		cout << "load obj with name " << (*a2).s << endl;
	}

	void test3(){
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
			Database db("t3.db");
			db.registerBeanClass<X>();
			db.dropModel();
			db.createModel();
			db.copyBean(x);
		}

		{
			Database db("t3.db");
			db.registerBeanClass<X>();
			bean_ptr<X> xptr=db.loadBean<X>(1);
			if( !(*xptr==x) )
				throw std::runtime_error("load failed");
		}

	}

    void test4() {
        WrappedBytes wrappedBytes;
        wrappedBytes.bytes = {'a', 'b', 'c', 'd', 'e'};
        
        {
          Database db("t4.db");
          db.registerBeanClass<WrappedBytes>();
          db.dropModel();
          db.createModel();
          db.copyBean(wrappedBytes);
        }

        {
          Database db("t4.db");
          db.registerBeanClass<WrappedBytes>();
          bean_ptr<WrappedBytes> wb_ptr = db.loadBean<WrappedBytes>(1);
          if (!(*wb_ptr == wrappedBytes))
              throw std::runtime_error("BLOB load failed");
        }
    }
};

int main()
{
	try{
		{
			Tester t;
			t.test1();
			t.test2();
			t.test3();
            t.test4();
		}
		cout << "tests passed\n";
	}catch(std::exception& e){
		cerr << "caught exception\n";
		cerr << e.what() << "\n";
		return 1;
	}
	catch(...){
		cerr << "caught strange exception\n";
		return 1;
	}
    return 0;
}/**/
