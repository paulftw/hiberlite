#ifndef DB_ERROR_H_INCLUDED
#define DB_ERROR_H_INCLUDED

namespace hiberlite{

class database_error : public std::runtime_error{
	public:
		inline database_error(const char* msg) : std::runtime_error(msg) {}
		inline database_error(std::string msg) : std::runtime_error(msg) {}
		inline static void database_assert(int result, shared_connection con){
			if(result==SQLITE_OK)
				return;
			throw database_error( sqlite3_errmsg(con->getSQLite3Ptr()) );
		}
};

} //namespace hiberlite

#endif // DB_ERROR_H_INCLUDED
