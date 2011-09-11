#ifndef CPPMODEL_H
#define CPPMODEL_H

namespace hiberlite{

class Scope{
	public:
		std::string _table, _prefix;
		int prefix_depth;

		inline Scope(std::string t, std::string p): _table(t)
												, _prefix(p)
												,prefix_depth(0)
		{}
		inline Scope() : prefix_depth(0) {}

		inline std::string table() { return _table; }
		inline std::string prefix() { return _prefix; }

		std::string name_column(std::string name);
		std::string full();
};

class Column{
	public:
		inline Column(std::string _n="", std::string _s="") : name(_n), storage_type(_s) {}
		std::string name;
		std::string storage_type;
};

class Table{
	public:
		Table();
		std::string name;
		std::map<std::string,Column> columns;
		void add(Column c);
		bool contains(std::string colname);
};

class Model : public std::map<std::string,Table> {
	public:
		void add(Table t);
};

}//namespace hiberlite

#endif // CPPMODEL_H
