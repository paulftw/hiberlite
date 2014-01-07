#ifndef BEANUPDATER_IMPL_HPP_INCLUDED
#define BEANUPDATER_IMPL_HPP_INCLUDED

namespace hiberlite{

template<class C>
inline void RowScope::addAssignment(const std::string name, db_atom<C>& atom){
	int id=FirstAtom+static_cast<int>(atoms.size());

	addSimpleAssign(name, "?"+Transformer::toSQLiteValue(id) );

	db_atom<C>* x=new db_atom<C>(atom);
	atoms.push_back(x);
}

template<class AV>
void UpdateBean::notifyInitWalk(AV& av)
{
	startRow(av.getScope().table(), av.getRootId(), Database::NULL_ID,0);
}

template<class AV>
void UpdateBean::notifyDoneWalk(AV& av)
{
	commitRow(av.getConnection(), av.getRootId());
}

template<class AV, class E, class S>
void UpdateBean::act(AV& av, collection_nvp<E,S> nvp ){

	sqlid_t index=0;
	S& stream=nvp.stream;

	av.diveTable(nvp.name);
		std::string tab=av.getScope().table();
		while(!stream.done()){
			E& el=stream.getNext();
				sqlid_t entry_id=Database::allocId(av.getConnection(), tab);

				startRow(tab,entry_id, curRow()->id,index);
					sql_nvp<E> el_nvp("item",el);
					av & el_nvp;
				commitRow(av.getConnection(), entry_id);
			index++;
		}
	av.pop();
}

template<class C>
void BeanUpdater::update(bean_key key, C& bean)
{
	startWalk( bean, key );
}

} //namespace hiberlite

#endif // BEANUPDATER_IMPL_HPP_INCLUDED
