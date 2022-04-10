#ifndef BEAN_PTR_H_INCLUDED
#define BEAN_PTR_H_INCLUDED

namespace hiberlite{

class Database;
class access;

template<class C>
class Registry;

template<class C>
class shared_res;

template<class C>
class real_bean : noncopyable {
	public:
		inline C* operator->();
		inline C* get();

		~real_bean();

		bean_key get_key() const { return key; }

		inline ESavePolicy getSavePolicy() const {
			return m_eSavePolicy;
		}
		inline void setSavePolicy( ESavePolicy save ) { m_eSavePolicy = save; }
		void save();

		void destroy();
		bool destroyed() const { return forgotten; }

		Database* database() const {
			return m_pDb;
		}


	protected:
		inline void loadLazy();

		Database* m_pDb = nullptr;
		bean_key key;
		C* obj;
		bool forgotten;
		ESavePolicy m_eSavePolicy = ESavePolicy_Default;
	private:
		friend class Registry<C>;
		real_bean( Database* pDb, const bean_key _key, C* _obj); //only Registry can create the real_bean
};

template<class C>
class bean_ptr : public shared_res< real_bean<C> >
{
	friend class hiberlite::access;
	template<class Archive>
	void hibernate(Archive & ar);

	friend class Registry<C>;
	bean_ptr( Database* pDb, bean_key k, rb_pair<C>* rbpair);

	sqlid_t tmp_id;

	public:
		bean_ptr( Database* pDb, bean_key k);

		bean_ptr();

		operator bool() const;

		bean_ptr(const bean_ptr<C>& other);
		bean_ptr<C>& operator=(const bean_ptr<C>& other);

		inline ESavePolicy getSavePolicy() const {
			return shared_res< real_bean<C> >::get_object()->getSavePolicy();
		}
		inline void setSavePolicy( ESavePolicy save ) { 
			return shared_res< real_bean<C> >::get_object()->setSavePolicy( save ); 
		}
		void save();
		void destroy();

		bool destroyed() {
			return shared_res< real_bean<C> >::get_object()->destroyed();
		}

		inline sqlid_t get_id();
		inline C& operator*();
		inline C* operator->();
};

}//namespace hiberlite

#endif // BEAN_PTR_H_INCLUDED
