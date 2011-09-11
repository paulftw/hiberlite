
namespace hiberlite{

template<class C>
class shared_res;

template<class C>
class shared_cnt_obj_pair : noncopyable {
protected:
	friend class shared_res<C>;

	C* res;
	int refCount;

	explicit inline shared_cnt_obj_pair(C* r) : res(r), refCount(0) {}

public:

	inline C* getRes(){
		return res;
	}
	inline virtual ~shared_cnt_obj_pair(){
		if(refCount)
			throw std::logic_error("resource is busy");
		delete res;
	}
	inline int getRefCount(){
		return refCount;
	}
	inline void inc_pair_users(){
		refCount++;
	}
	inline void dec_pair_users(){
		refCount--;
	}
};

template<class C>
class shared_res{
protected:
	shared_cnt_obj_pair<C>* res;

	inline void freeRes(){
		if(!res)
			return;
		res->dec_pair_users();
		if(res->getRefCount()==0)
			delete res;
	}
	inline void takeRes(shared_cnt_obj_pair<C>* r){
		res=r;
		if(!res)
			return;
		res->inc_pair_users();
	}

public:
	inline explicit shared_res(C* tg) : res(NULL) {
		if(!tg)
			throw std::logic_error("no reason to manage NULL object");
		shared_cnt_obj_pair<C>* share=new shared_cnt_obj_pair<C>(tg);
		takeRes(share);
	}

	inline shared_res() : res(NULL) {}

	inline shared_res(const shared_res<C>& x) : res(NULL) {
		takeRes(x.res);
	}

	inline virtual ~shared_res(){
		freeRes();
	}

	inline shared_res<C>& operator=(const shared_res<C>& x){
		if(x.res!=res){
			freeRes();
			takeRes(x.res);
		}
		return *this;
	}

	inline C* get_object(){
		if(!res)
			return NULL;
		return res->getRes();
	}

	inline C* operator->(){
		return get_object();
	}

	inline int get_ref_count(){
		if(!res)
			return 0;
		return res->getRefCount();
	}

	inline bool operator==(const shared_res<C>& x) const{
		return x.res==res;
	}
	inline bool operator<(const shared_res<C>& x) const{
		return res<x.res;
	}
	inline operator bool() const{
		return res!=NULL;
	}
};

class autoclosed_con{
	protected:
		sqlite3* con;

	public:
		inline autoclosed_con(sqlite3* _con) : con(_con) {}
		inline ~autoclosed_con(){
			if(con)
				sqlite3_close(con);
		}

		inline sqlite3* getSQLite3Ptr() { return con; }
};

typedef shared_res<autoclosed_con> shared_connection;

class bean_key{
public:
	sqlid_t id;
	shared_connection con;

	bean_key(sqlid_t _id, shared_connection _con) : id(_id), con(_con) {}
	bean_key(shared_connection _con, sqlid_t _id) : id(_id), con(_con) {}
	~bean_key() {}
	bean_key();

	bool operator<(const bean_key& k) const{
		return (id<k.id) || ((id==k.id) && (con<k.con));
	}
};

} //namespace hiberlite
