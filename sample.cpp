#include "hiberlite.h"

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;

#define TEST_OLD_DB 0

class Id {
	friend class hiberlite::access;

	template<class Archive>	
	void hibernate( Archive & ar )
	{
		ar & HIBERLITE_NVP( m_id );
		//ar & HIBERLITE_NVP( bio2 );
	}
public:
	string m_id;
};

HIBERLITE_EXPORT_CLASS( Id )

class Person{
  friend class hiberlite::access;
  template<class Archive>
  void hibernate(Archive & ar)
  {
	  ar & HIBERLITE_NVP( id );
    ar & HIBERLITE_NVP( name );
    ar & HIBERLITE_NVP( wname );
    ar & HIBERLITE_NVP(age);
    ar & HIBERLITE_NVP( bio );
    //ar & HIBERLITE_NVP( bio2 );
  }
public:
	Person() {}
	~Person() {}

	Id id;
  string name;
  wstring wname;
  double age;
  vector<string> bio;
  vector<wstring> bio2;
};

HIBERLITE_EXPORT_CLASS(Person)

void createDB()
{
  hiberlite::Database db("sample.db");
  //register bean classes
  db.registerBeanClass<Person>();

#if TEST_OLD_DB
  std::vector<std::string> tableNames = db.checkModel();
  db.checkCreateModel();
#else
  //drop all tables beans will use
  db.dropModel();
  //create those tables again with proper schema
  db.createModel();
#endif

  const char* names[5]={"Stanley Marsh", "Kyle Broflovski", "Eric Theodore Cartman", "Kenneth McCormick", "Leopold Stotch"};

  for(unsigned int i=0;i<5;i++) {
    Person x;
    x.name=names[i%5];
	x.wname = hiberlite::utf8ToWstring( names[i % 5] );	
    x.age=14+i*0.1;
    x.bio.push_back("Hello");
    x.bio.push_back("world");
    x.bio.push_back("!");

    x.bio2.push_back( L"wHello" );
	 x.bio2.push_back( L"wworld" );
	 x.bio2.push_back( L"!" );

    hiberlite::bean_ptr<Person> p=db.copyBean(x);	//create a managed copy of the object
  }
}

/**数据量稍微一大，就慢的要命。
https://blog.csdn.net/majiakun1/article/details/46607163
加了外部使用事务的接口，能快一点
*/
void createDBMuch()
{
	hiberlite::Database db( "sample.db", hiberlite::EDBSynchronous_Normal );
	//register bean classes
	db.registerBeanClass<Person>();

#if TEST_OLD_DB
	std::vector<std::string> tableNames = db.checkModel();
	db.checkCreateModel();
#else
	//drop all tables beans will use
	db.dropModel();
	//create those tables again with proper schema
	db.createModel();
#endif

	const char* names[5] = { "Stanley Marsh", "Kyle Broflovski", "Eric Theodore Cartman", "Kenneth McCormick", "Leopold Stotch" };

	auto t = db.transactionGuard();
	db.beginTransaction(false);

	for( int j = 0; j < 500; ++j ) {
		for( unsigned int i = 0; i < 5; i++ ) {
			Person x;
			x.name = names[i % 5] ;
			x.name += " " + std::to_string( j );
			x.id.m_id = x.name;
			x.wname = hiberlite::utf8ToWstring( names[i % 5] );
			x.age = 14 + i * 0.1;
			x.bio.push_back( "Hello" );
			x.bio.push_back( "world" );
			x.bio.push_back( "!" );

			x.bio2.push_back( L"wHello" );
			x.bio2.push_back( L"wworld" );
			x.bio2.push_back( L"!" );

			db.copyBeanToDB( x );
			//hiberlite::bean_ptr<Person> p = db.copyBean( x );	//create a managed copy of the object
		}
	}
	
	db.endTransaction();
}

void printDB()
{
  hiberlite::Database db("sample.db", hiberlite::EDBSynchronous_Normal );
  db.registerBeanClass<Person>();

  auto t = db.transactionGuard();
  cout << string(15,'=')+"\nreading the DB\n";

  vector< hiberlite::bean_ptr<Person> > v=db.getAllBeans<Person>( hiberlite::ESavePolicy_ExitNoSave );

  cout << "found " << v.size() << " persons in the database:\n";

  for(size_t j=0;j<v.size();j++){
    cout << j << "[name=" << v[j]->name << "\t";
	cout << "wname=" << hiberlite::wstringToUtf8(v[j]->wname) << "\t";
    cout << "age=" << v[j]->age << "\t";
    cout << "bio={";
    for(size_t i=0;i<v[j]->bio.size();i++)
      i && cout << ", ", cout << v[j]->bio[i];
    cout << "}\t";

	cout << "bio2={";
	for( size_t i = 0; i < v[j]->bio2.size(); i++ )
		i && cout << ", ", cout << hiberlite::wstringToUtf8( v[j]->bio2[i]);
	cout << "}]\n";
  }
}

void modifyDB()
{
  cout << string( 15, '=' ) + "\nmodify the DB\n";
  hiberlite::Database db("sample.db");
  db.registerBeanClass<Person>();

  vector< hiberlite::bean_ptr<Person> > v=db.getAllBeans<Person>();

  if( v.size() > 0 ) {
	  cout << v[0]->name << " will be deleted.\n";
	  v[0].destroy();

	  if( v[0] ) {
		  cout << "error: v[0] should be null!";
	  }
  }

  if( v.size() > 1 ) {
	  cout << v[1]->name << " becomes 1 year older.\n\n";
	  v[1]->age += 1;
  }

  vector< hiberlite::bean_ptr<Person> > v2 = db.getAllBeans<Person>();
}


void modifyDBMuch()
{
	cout << string( 15, '=' ) + "\nmodify the DB\n";
	hiberlite::Database db( "sample.db", hiberlite::EDBSynchronous_Normal );
	db.registerBeanClass<Person>();

	auto t = db.transactionGuard();
	vector< hiberlite::bean_ptr<Person> > v = db.getAllBeans<Person>();

	cout << "every record becomes 1 year older.\n\n";
	for( auto i: v ) {
		i->age += 1;
	}

	cout << "every record becomes 1 year older.\n\n";
	for( auto i : v ) {
		i->age += 1;
	}

	if( v.size() > 0 ) {
		cout << v[0]->name << " will be deleted.\n";
		v[0].destroy();

		if( v[0] ) {
			cout << "error: v[0] should be null!";
		}
	}
	//cout << "every record will be deleted.\n\n";
	//for( auto i : v ) {
	//	i.destroy();
	//}

	vector< hiberlite::bean_ptr<Person> > v2 = db.getAllBeans<Person>( hiberlite::ESavePolicy_ExitNoSave );
}

int main()
{
  //createDB();
  createDBMuch();
  printDB();
  modifyDBMuch();
  //modifyDB();
  printDB();
  //modifyDB();
  //printDB();
  return 0;
}
