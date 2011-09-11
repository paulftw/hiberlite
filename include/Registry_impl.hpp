namespace hiberlite{

template<class C>
rb_pair<C>::~rb_pair()
{
	Registry<C>::notifyRBDies(this);
}

template<class C>
rb_pair<C>::rb_pair(real_bean<C>* rbean) : shared_cnt_obj_pair< real_bean<C> >(rbean)
{
}

template<class C>
bean_key rb_pair<C>::get_key()
{
	return shared_cnt_obj_pair< real_bean<C> >::res->get_key();
}

template<class C>
bean_ptr<C> Registry<C>::createBeanPtr(bean_key key, C* obj)
{
	if(key.id==Database::NULL_ID)
		return bean_ptr<C>(key,NULL);

	real_bean<C>* rb=new real_bean<C>(key,obj);
	rb_pair<C>* para=new rb_pair<C>(rb);

	if( rbpairs.find(key)!=rbpairs.end() )
		throw std::logic_error("bean already exists");

	rbpairs[key]=para;

	bean_ptr<C> ans(key, para);
	return ans;
}

template<class C>
bean_ptr<C> Registry<C>::get(const bean_key key)
{
	typename std::map<bean_key,rb_pair<C>* >::iterator it;
	it=rbpairs.find(key);
	if(it==rbpairs.end())
		return createBeanPtr(key,NULL);
	else
		return bean_ptr<C>(key, it->second);
}

template<class C>
bool Registry<C>::has(const bean_key key)
{
	return rbpairs.find(key)!=rbpairs.end();
}

template<class C>
void Registry<C>::notifyRBDies(rb_pair<C>* r){
	rbpairs.erase(r->get_key());
}

} // namespace hiberlite
