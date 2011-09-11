
#include "hiberlite.h"

namespace hiberlite{

Database::Database() : mx(NULL)
{
}

Database::Database(std::string fname) : mx(NULL)
{
	open(fname);
}

Database::~Database()
{
	close();
}

void Database::open(std::string fname)
{
	sqlite3* db=NULL;

	try{
		//TODO UTF-8 string
		int rc=sqlite3_open(fname.c_str(), &db);
		if(rc!=SQLITE_OK)
			throw database_error( std::string("database error: ")+sqlite3_errmsg(db) );
		con=shared_connection(new autoclosed_con(db));

	}catch(std::runtime_error e){
		if(db)
			sqlite3_close(db);
		throw e;
	}
}

void Database::close()
{
	if(mx) {
		delete mx;
		mx=NULL;
	}
}

std::vector<std::string> Database::checkModel()
{
	//TODO checkModel
	std::vector<std::string> ans;
	return ans;
}

void Database::dropModel()
{
	if(!mx)
		throw std::logic_error("register bean classes first");
	Model mdl=mx->getModel();
	for(Model::iterator it=mdl.begin();it!=mdl.end();it++){
		Table& t=it->second;
		std::string query="DROP TABLE IF EXISTS "+t.name+";";
		dbExecQuery(query);
	}
}

void Database::createModel()
{
	if(!mx)
		throw std::logic_error("register bean classes first");
	Model mdl=mx->getModel();
	for(Model::iterator it=mdl.begin();it!=mdl.end();it++){
		Table& t=it->second;
		std::string query="CREATE TABLE "+t.name+" (";
		bool needComma=false;
		for(std::map<std::string,Column>::iterator c=t.columns.begin();c!=t.columns.end();c++){
			if(needComma)
				query+=", ";
			needComma=true;
			Column& col=c->second;
			query += col.name + " ";
			if(col.name==HIBERLITE_PRIMARY_KEY_COLUMN)
				query+="INTEGER PRIMARY KEY AUTOINCREMENT";
			else
				query+=col.storage_type;
		}
		query +=");";
		dbExecQuery(query);
	}
}

sqlid_t Database::allocId(shared_connection c, std::string table)
{
	//THREAD critical call
	char* err_msg=NULL;
	std::string query="INSERT INTO "+table+" ("+HIBERLITE_PRIMARY_KEY_COLUMN+") VALUES (NULL);";
	HIBERLITE_HL_DBG_DO( std::cout << "exec: " << query << std::endl; )
	int rc=sqlite3_exec(c->getSQLite3Ptr(),query.c_str(),NULL, NULL, &err_msg );
	if(err_msg)
		throw database_error(err_msg);
	database_error::database_assert(rc, c);

	return sqlite3_last_insert_rowid(c->getSQLite3Ptr());
}

void Database::dbExecQuery(shared_connection con, std::string query)
{
	char* err_msg=NULL;
	HIBERLITE_HL_DBG_DO( std::cout << "exec: " << query << std::endl; )
	int rc=sqlite3_exec(con->getSQLite3Ptr(),query.c_str(),NULL, NULL, &err_msg );
	if(err_msg){
		std::string msg=err_msg;
		sqlite3_free(err_msg);
		throw database_error(msg);
	}
	database_error::database_assert(rc, con);
}

sqlid_t Database::allocId(std::string table){
	return allocId(con, table);
}

void Database::dbExecQuery(std::string query){
	return dbExecQuery(con, query);
}

std::vector<sqlid_t> Database::dbSelectIds(shared_connection con, const std::string table, const std::string condition, const std::string orderBy)
{
	std::string where;
	if(condition.size())
		where=" WHERE "+condition;
	std::string order;
	if(orderBy.size())
		order=" ORDER BY "+orderBy;
	std::string query="SELECT "+std::string(HIBERLITE_PRIMARY_KEY_COLUMN)
			+" FROM "+table
			+where
			+order
			+";";
	SQLiteSelect sel(con, query);
	std::vector<sqlid_t> ans;
	while(sel.step())
		ans.push_back( sel.get_int64(0) );
	return ans;
}

std::vector<sqlid_t> Database::dbSelectChildIds(shared_connection con, std::string table, sqlid_t parent)
{
	return dbSelectIds(con, table, std::string(HIBERLITE_PARENTID_COLUMN)+"="+Transformer::toSQLiteValue(parent)
									, HIBERLITE_ENTRY_INDEX_COLUMN);
}

std::vector<sqlid_t> Database::dbSelectChildIds(std::string table, sqlid_t parent)
{
	return dbSelectChildIds(con,table,parent);
}


} //namespace hiberlite;
