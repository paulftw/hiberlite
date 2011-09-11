#ifndef REGISTRY_H_INCLUDED
#define REGISTRY_H_INCLUDED

namespace hiberlite{

class Database;

template<class C> class bean_ptr;
template<class C> class Registry;
template<class C> class real_bean;

template<class C>
class rb_pair : public shared_cnt_obj_pair< real_bean<C> > {
protected:
	friend class Registry<C>;

	inline rb_pair(real_bean<C>* rbean);

public:
	inline bean_key get_key();
	inline virtual ~rb_pair();
};


template<class C>
class Registry{
	protected:
		friend class rb_pair<C>;

		static std::map<bean_key,rb_pair<C>* > rbpairs;

		static void notifyRBDies(rb_pair<C>* r);

	public:
		static bean_ptr<C> get(const bean_key key);
		static bool has(const bean_key key);

		static bean_ptr<C> createBeanPtr(bean_key key, C* obj);
};

template<class C>
std::map< bean_key, rb_pair<C>* > Registry<C>::rbpairs;

} //namespace hiberlite

#endif // REGISTRY_H_INCLUDED
