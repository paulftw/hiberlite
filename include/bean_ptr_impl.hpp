namespace hiberlite{

template<class C>
real_bean<C>::real_bean(const bean_key _key, C* _obj) : key(_key), obj(_obj), forgotten(false)
{}

template<class C>
real_bean<C>::~real_bean()
{
	save();
	delete obj;
}

template<class C>
void real_bean<C>::destroy() {
	if(forgotten)
		return;
	Database::dbDelete(key, *obj);
	delete obj;
	forgotten=true;
	obj=NULL;
	key.id=Database::NULL_ID;
}

template<class C>
void real_bean<C>::save() {
	if(forgotten)
		return;
	if(!obj)
		return;
	Database::dbUpdate(key, *obj);
}

template<class C>
C* real_bean<C>::operator->()
{
	if(forgotten)
		return NULL;
	loadLazy();
	if(!obj)
		throw std::runtime_error("NULL pointer exception!");
	return obj;
}

template<class C>
C* real_bean<C>::get()
{
	if(forgotten)
		return NULL;
	loadLazy();
	if(!obj)
		throw std::runtime_error("NULL pointer exception!");
	return obj;
}

template<class C>
inline void real_bean<C>::loadLazy()
{
	if(!obj && key.id!=Database::NULL_ID)
		obj=Database::dbLoad<C>(key);
}

template<class C>
bean_ptr<C>::bean_ptr(bean_key k, rb_pair<C>* para)
{
	this->takeRes(para);
}

template<class C>
bean_ptr<C>::bean_ptr(const bean_ptr<C>& other) : shared_res< real_bean<C> >(other)
{
}

template<class C>
bean_ptr<C>& bean_ptr<C>::operator=(const bean_ptr<C>& other)
{
	shared_res< real_bean<C> >::operator=( other );
	return *this;
}

template<class C>
bean_ptr<C>::bean_ptr(bean_key k)
{
	*this=Registry<C>::get(k);
}

template<class C>
bean_ptr<C>::bean_ptr()
{
}

template<class C>
bean_ptr<C>::operator bool() const {
	return get_id()!=Database::NULL_ID;
}

template<class C> template<class Archive>
void bean_ptr<C>::hibernate(Archive & ar)
{
	tmp_id=get_id();

	if(tmp_id!=Database::NULL_ID
			&& ar.getConnection()!=shared_res< real_bean<C> >::get_object()->get_key().con)
		throw std::logic_error("saving the bean from different database");

	ar & hiberlite::sql_nvp< sqlid_t > ("id", tmp_id );
	if(ar.is_loading())
		*this=Registry<C>::get( bean_key(ar.getConnection(), tmp_id) );
}

template<class C>
C& bean_ptr<C>::operator*() {
	return *(shared_res< real_bean<C> >::get_object()->get());
}

template<class C>
C* bean_ptr<C>::operator->() {
	return shared_res< real_bean<C> >::get_object()->get();
}

template<class C>
void bean_ptr<C>::destroy() {
	shared_res< real_bean<C> >::get_object()->destroy();
}

template<class C>
void bean_ptr<C>::save() {
	shared_res< real_bean<C> >::get_object()->save();
}

template<class C>
sqlid_t bean_ptr<C>::get_id() {
	if( !shared_res< real_bean<C> >::get_object() )
		return Database::NULL_ID;
	return shared_res< real_bean<C> >::get_object()->get_key().id;
}

} //namespace hiberlite
