#ifndef VISITOR_TMPL_IMPL_HPP_INCLUDED
#define VISITOR_TMPL_IMPL_HPP_INCLUDED

namespace hiberlite{

template<class C>
AVisitor<C>::AVisitor(C* _act, int _purp) : actor(_act)
										, purpose(_purp)
{}

template<class C>
bool AVisitor<C>::is_loading(){
	return purpose==LOADING;
}

template<class C>
bool AVisitor<C>::is_saving(){
	return purpose==SAVING;
}

template<class C>
bool AVisitor<C>::is_model(){
	return purpose==MODEL;
}

template<class C>
bool AVisitor<C>::is_deleting(){
	return purpose==DELETING;
}

template<class C>
AVisitor<C>::~AVisitor() {}

template<class C>
shared_connection AVisitor<C>::getConnection()
{
	return rootKey.con;
}

template<class C>
sqlid_t AVisitor<C>::getRootId()
{
	return rootKey.id;
}

template<class C> template<class Y>
void AVisitor<C>::startWalk(Y& obj, bean_key key)
{
	rootKey=key;
	stack=std::stack<Scope>();
	std::string ClassName=Database::getClassName<Y>();
	scope=Scope(ClassName,"");

	actor->notifyInitWalk(*this);
		walk(obj);
	actor->notifyDoneWalk(*this);
}

template<class C> template<class Y>
void AVisitor<C>::walk(Y& obj)
{
	hibernate(*this, obj, static_cast<const unsigned int>(0));
}

template<class C> template<class X>
AVisitor<C>& AVisitor<C>::operator&(sql_nvp<X> nvp)
{
	divePrefix(nvp.name);
		walk(nvp.value);
	pop();
	return *this;
}

template<class C> template<class X>
AVisitor<C>& AVisitor<C>::operator&(db_atom<X> atom)
{
	actor->act(*this,atom);
	return *this;
}

template<class C> template<class E, class S>
AVisitor<C>& AVisitor<C>::operator&(collection_nvp<E,S> nvp)
{
	actor->act(*this,nvp);
	return *this;
}

template<class Vis>
void AVisitor<Vis>::divePrefix(std::string name){
	pushAndSubPrefix(name);
}

template<class Vis>
void AVisitor<Vis>::diveTable(std::string name){
	pushAndSubTable(name);
}

template<class Vis>
void AVisitor<Vis>::pop(){
	popScope();
}

template<class C>
void AVisitor<C>::popScope()
{
	if(!stack.size())
		throw std::runtime_error("popScope with empty stack");
	scope=stack.top();
	stack.pop();
}

template<class C>
void AVisitor<C>::pushAndSubTable(std::string name)
{
	stack.push(scope);
	scope._table=scope.full()+"_"+name;
	scope._prefix="";
	scope.prefix_depth=0;
}

template<class C>
void AVisitor<C>::pushAndSubPrefix(std::string prefix)
{
	stack.push(scope);
	if(scope.prefix().size())
		scope._prefix+="_"+prefix;
	else
		scope._prefix+=prefix;
	scope.prefix_depth++;
}

} //namespace hiberlite

#endif // VISITOR_TMPL_IMPL_HPP_INCLUDED
