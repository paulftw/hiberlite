#ifndef CHILDKILLER_IMPL_HPP_INCLUDED
#define CHILDKILLER_IMPL_HPP_INCLUDED

namespace hiberlite{

template<class AV>
void KillChildren::notifyInitWalk(AV& av)
{
	rowId=std::stack<sqlid_t>();
	rowId.push(av.getRootId());
}

template<class AV>
void KillChildren::notifyDoneWalk(AV& av)
{
}

template<class AV, class E, class S>
void KillChildren::act(AV& av, collection_nvp<E,S> nvp )
{
	av.diveTable(nvp.name);
		std::string tab=av.getScope().table();
		std::vector<sqlid_t> children=Database::dbSelectChildIds(av.getConnection(), tab, rowId.top());
		for(size_t i=0;i<children.size();i++){
			E* entry;
			construct(av,&entry,static_cast<unsigned int>(0));
				rowId.push(children[i]);
					sql_nvp<E> el("item",*entry);
					av & el;
				rowId.pop();
			destroy(*this, entry, static_cast<unsigned int>(0));
		}
	av.pop();

	Database::dbDeleteRows(av.getConnection(),tab,HIBERLITE_PARENTID_COLUMN,rowId.top());
}

template<class C>
void ChildKiller::killChildren(bean_key key, C& bean)
{
	startWalk(bean, key);
}

} //namespace hiberlite


#endif // BEANUPDATER_IMPL_HPP_INCLUDED
