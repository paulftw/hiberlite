#ifndef _NVP_H_
#define _NVP_H_

#include "common.h"

namespace hiberlite{

template<class C>
class sql_nvp{
	public:
		std::string name;
		C& value;
		std::string search_key;

		sql_nvp(std::string _name, C& _value, std::string search="") : name(_name), value(_value), search_key(search) {}
};

template<class E,class Stream>
class collection_nvp{
	public:
		std::string name;
		Stream stream;
		collection_nvp(std::string _name, Stream _s) : name(_name), stream(_s) {}
};

class abstract_atom{
	public:
		virtual void bindValue(sqlite3_stmt* stmt, int col)=0;
		inline virtual ~abstract_atom() {}
};

template<class C>
class db_atom : public abstract_atom{
	public:
		C& val;
		db_atom(C& x) : val(x) {}
		inline virtual ~db_atom() {}

		inline std::string sqliteStorageClass();

		template<class Stmt,class Arg>
		void loadValue(Stmt& res, Arg& arg);

		inline virtual void bindValue(sqlite3_stmt* stmt, int col);
};

} //namespace hiberlite

#endif
