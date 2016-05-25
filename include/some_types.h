#ifndef SOM_TYPES_H_INCLUDED
#define SOM_TYPES_H_INCLUDED

namespace hiberlite{

template<class E, class C>
class stl_stream_adapter{
		C& ct;
		typename C::iterator it;
		E xx;
	public:
		void startLoadFromDb(){
			ct.clear();
			it=ct.begin();
		}
		stl_stream_adapter(C& _ct) : ct(_ct){
			it=ct.begin();
		}
		void putNext(E& el){
			ct.insert(ct.end(), el);
			it=ct.end();
		}
		E& getNext(){
			xx=*it++;
			return xx;
		}
		bool done(){
			return it==ct.end();
		}
};

template<class A, class E>
void hibernate(A& ar, std::vector<E>& v, const unsigned int)
{
	collection_nvp<E,stl_stream_adapter<E,std::vector<E> > > body( "items", stl_stream_adapter<E,std::vector<E> >(v) );
	ar & body;
}

template<class A, class First, class Second>
void hibernate(A& ar, std::pair<First,Second>& m, const unsigned int)
{
	sql_nvp<First> f("first", m.first);
	sql_nvp<Second> s("second", m.second);
	ar & f;
	ar & s;
}

template<class A, class Key>
void hibernate(A& ar, std::set<Key>& m, const unsigned int)
{
	typedef Key ElType;
	collection_nvp<ElType,stl_stream_adapter<ElType,std::set<Key> > >
			body( "items", stl_stream_adapter<ElType,std::set<Key> >(m) );
	ar & body;
}

template<class A, class Key>
void hibernate(A& ar, std::list<Key>& m, const unsigned int)
{
	typedef Key ElType;
	collection_nvp<ElType,stl_stream_adapter<ElType,std::list<Key> > >
			body( "items", stl_stream_adapter<ElType,std::list<Key> >(m) );
	ar & body;
}

template<class K, class D>
class stl_map_adapter{
		typedef std::pair<const K,D> RType;
		typedef std::pair<K,D> WType;
		typedef std::map<K,D> C;
		C& ct;
		typename C::iterator it;

		WType xx;
	public:
		void startLoadFromDb(){
			ct.clear();
			it=ct.begin();
		}
		stl_map_adapter(C& _ct) : ct(_ct){
			it=ct.begin();
		}
		void putNext(WType& el){
			ct.insert(el);
			it=ct.end();
		}
		WType& getNext(){
			xx=*it++;
			return xx;
		}
		bool done(){
			return it==ct.end();
		}
};
template<class A, class Key, class Data>
void hibernate(A& ar, std::map<Key,Data>& m, const unsigned int)
{
	typedef std::pair<Key,Data> ElType;
	collection_nvp<ElType,stl_map_adapter<Key,Data> >
			body( "items", stl_map_adapter<Key,Data>(m) );
	ar & body;
}

#define HIBERLITE_DEF_DB_ATOM(T, GetFromDB, PutToDB, StorClass) \
template<class A> void hibernate(A& ar, T& value, const unsigned int)	\
	{	ar & db_atom<T>(value);	}										\
template<> inline std::string db_atom<T>::sqliteStorageClass()					\
	{	return StorClass;	}											\
template<> template<class Stmt,class Arg> void db_atom<T>::loadValue(Stmt& res, Arg& arg)		\
	{	val=res.GetFromDB(arg);	}										\
template<> inline void db_atom<T>::bindValue(sqlite3_stmt* stmt, int col)		\
	{	PutToDB(stmt,col,val);	}

#define HIBERLITE_DEF_INT_ATOM(T) HIBERLITE_DEF_DB_ATOM(T, get_int, sqlite3_bind_int, "INTEGER")

HIBERLITE_DEF_INT_ATOM(int)
HIBERLITE_DEF_INT_ATOM(unsigned int)
HIBERLITE_DEF_INT_ATOM(char)
HIBERLITE_DEF_INT_ATOM(unsigned char)
HIBERLITE_DEF_INT_ATOM(short)
HIBERLITE_DEF_INT_ATOM(unsigned short)

HIBERLITE_DEF_DB_ATOM(long long, get_int64, sqlite3_bind_int64, "INTEGER")

HIBERLITE_DEF_DB_ATOM(long int, get_int64, sqlite3_bind_int64, "INTEGER")
HIBERLITE_DEF_DB_ATOM(unsigned long int, get_int64, sqlite3_bind_int64, "INTEGER")
HIBERLITE_DEF_DB_ATOM(unsigned long long, get_int64, sqlite3_bind_int64, "INTEGER")

HIBERLITE_DEF_DB_ATOM(float, get_double, sqlite3_bind_double, "REAL")
HIBERLITE_DEF_DB_ATOM(double, get_double, sqlite3_bind_double, "REAL")

#undef HIBERLITE_DEF_INT_ATOM
#undef HIBERLITE_DEF_DB_ATOM

template<class A>
void hibernate(A& ar, std::string& value, const unsigned int){
	ar & db_atom<std::string>(value);
}

template<> template<class Stmt,class Arg>
void db_atom<std::string>::loadValue(Stmt& res, Arg& arg){
	val=std::string(	(const char*)(res.get_text(arg))	);
}

template<>
inline std::string db_atom<std::string>::sqliteStorageClass(){
	return "TEXT";
}
template<>
inline void db_atom<std::string>::bindValue(sqlite3_stmt* stmt, int col){
	sqlite3_bind_text(stmt,col,val.c_str(),-1,SQLITE_TRANSIENT);
}


} //namespace hiberlite

#endif // SOM_TYPES_H_INCLUDED
