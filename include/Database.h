#ifndef DATABASE_H
#define DATABASE_H

namespace hiberlite{

class ModelExtractor;
class UpdateBean;
class Database;

namespace detail {
	class DatabaseTransaction 
	{
	public:
		inline DatabaseTransaction();
		inline ~DatabaseTransaction();
		inline DatabaseTransaction( const DatabaseTransaction& );
		inline DatabaseTransaction& operator=( const DatabaseTransaction& );

	private:
		friend class Database;
		inline DatabaseTransaction( shared_connection key, bool bRollback = true );

		bool m_bValid = true;
		//bool m_bRollback = true;
		shared_connection m_connection;
	};
}

class Database : noncopyable
{
	protected:
		friend class UpdateBean;
		friend class KillChildren;
		friend class LoadBean;
		friend class detail::DatabaseTransaction;

		template<class C>
		friend class real_bean;

		ModelExtractor* mx;

		template<class C>
		static void dbDelete(bean_key key, C& bean, bool bUseTransaction );

		template<class C>
		static void dbUpdate(bean_key key, C& bean,  bool bUseTransaction );

		template<class C>
		static C* dbLoad(bean_key key);

		static sqlid_t allocId(shared_connection c, std::string table);
		sqlid_t allocId(std::string table);

		static void dbExecQuery(shared_connection con, std::string query);
		void dbExecQuery(std::string query);

		static std::vector<sqlid_t> dbSelectIds(shared_connection con, const std::string table,
														const std::string condition, const std::string orderBy);

		static std::vector<sqlid_t> dbSelectChildIds(shared_connection con, std::string table, sqlid_t parent);
		inline std::vector<sqlid_t> dbSelectChildIds(std::string table, sqlid_t parent);

		shared_connection con;

		template<class C>
		static void dbDeleteRows( shared_connection con, std::string table, std::string column, C value );

		inline bool shouldInsideUseTransaction() const;

	public:
		static const sqlid_t NULL_ID=-1;

		template<class C>
		static std::string getClassName();

		Database();
		Database(std::string fname);
		virtual ~Database();

		void open(std::string fname);
		void close();

		template<class C>
		inline void registerBeanClass();

		Model getModel();
		inline shared_connection getConnection() { return con; }

		std::vector<std::string> checkModel();
		void dropModel();
		void createModel();
		void checkCreateModel();

		/** 外部使用事务，内部就不再使用事务了，能快一点 */
		inline bool beginTransaction( bool bRollback = true );
		inline bool endTransaction();

		template<class C>
		bean_ptr<C> loadBean(sqlid_t objId);

		template<class C>
		std::vector<sqlid_t> getBeanIds();

		template<class C>
		std::vector< bean_ptr<C> > getAllBeans();

		template<class C>
		bean_ptr<C> copyBean(const C& c);

		template<class C>
		bean_ptr<C> createBean();

		template<class C>
		bean_ptr<C> manageBean(C* ptr);

		detail::DatabaseTransaction m_transaction;
};

} //namespace hiberlite;


#endif // DATABASE_H
