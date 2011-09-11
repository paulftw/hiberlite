#ifndef SQLITESTMT_H
#define SQLITESTMT_H

namespace hiberlite{

class statement_ptr{
	public:
		inline statement_ptr(sqlite3_stmt* st) : obj(st) {}
		inline ~statement_ptr() { sqlite3_finalize(obj); }

		sqlite3_stmt* get_stmt() { return obj; }
	private:
		sqlite3_stmt* obj;

		inline statement_ptr(const statement_ptr& st);
		inline statement_ptr& operator=(const statement_ptr& st);
};

typedef shared_res<statement_ptr> shared_stmt;

class SQLiteSelect
{
	public:
		SQLiteSelect(shared_connection con, std::string query);

		bool step();

		virtual ~SQLiteSelect();

		const void* get_blob(int iCol);
		int get_bytes(int iCol);
		double get_double(int iCol);
		int get_int(int iCol);
		sqlid_t get_int64(int iCol);
		const unsigned char *get_text(int iCol);
		int get_type(int iCol);
		int column_count();
		std::string get_name(int N);

	protected:
		bool active;
		shared_stmt statement;
		shared_connection con;
	private:
};

} //namespace hiberlite

#endif // SQLITESTMT_H
