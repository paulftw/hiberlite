#include "hiberlite.h"

namespace hiberlite{

SQLiteSelect::SQLiteSelect(shared_connection _con, std::string query) : con(_con)
{
	HIBERLITE_HL_DBG_DO( std::cout << "query: " << query << std::endl; )
	active=false;
	sqlite3_stmt* stmt_ptr=NULL;
	sqlite3* db=con->getSQLite3Ptr();
	const char* foob;
	int rc=sqlite3_prepare_v2(db,query.c_str(),-1,&stmt_ptr,&foob);
	database_error::database_assert(rc, con);
	statement=shared_stmt( new statement_ptr(stmt_ptr) );
}

SQLiteSelect::~SQLiteSelect()
{
}

bool SQLiteSelect::step()
{
	int rc=sqlite3_step(statement->get_stmt());
	if(rc==SQLITE_DONE)
		return false;
	if(rc==SQLITE_ROW){
		active=true;
		return true;
	}
	database_error::database_assert(rc, con);
	throw database_error("really strange - sqlite3_step returns SQLITE_OK");
}

const void* SQLiteSelect::get_blob(int iCol){
	if(!active)
		throw std::runtime_error("step() was not called before column access");
	return sqlite3_column_blob(statement->get_stmt(), iCol);
}

int SQLiteSelect::get_bytes(int iCol){
	if(!active)
		throw std::runtime_error("step() was not called before column access");
	return sqlite3_column_bytes(statement->get_stmt(), iCol);
}

double SQLiteSelect::get_double(int iCol){
	if(!active)
		throw std::runtime_error("step() was not called before column access");
	return sqlite3_column_double(statement->get_stmt(), iCol);
}

int SQLiteSelect::get_int(int iCol){
	if(!active)
		throw std::runtime_error("step() was not called before column access");
	return sqlite3_column_int(statement->get_stmt(), iCol);
}

sqlid_t SQLiteSelect::get_int64(int iCol){
	if(!active)
		throw std::runtime_error("step() was not called before column access");
	return sqlite3_column_int64(statement->get_stmt(), iCol);
}

const unsigned char *SQLiteSelect::get_text(int iCol){
	if(!active)
		throw std::runtime_error("step() was not called before column access");
	return sqlite3_column_text(statement->get_stmt(), iCol);
}

int SQLiteSelect::get_type(int iCol){
	if(!active)
		throw std::runtime_error("step() was not called before column access");
	return sqlite3_column_type(statement->get_stmt(), iCol);
}

std::string SQLiteSelect::get_name(int N){
	if(!active)
		throw std::runtime_error("step() was not called before column access");
	return sqlite3_column_name(statement->get_stmt(), N);
}

int SQLiteSelect::column_count(){
	if(!active)
		throw std::runtime_error("step() was not called before column access");
	return sqlite3_column_count(statement->get_stmt());
}


}

//namespace hiberlite
