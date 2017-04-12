#ifndef BeanLoader_IMPL_HPP_INCLUDED
#define BeanLoader_IMPL_HPP_INCLUDED

namespace hiberlite{

template<class AV>
void LoadBean::notifyInitWalk(AV& av)
{
	while(!stmt.empty())
		stmt.pop();

	sqlid_t curId=av.getRootId();

	std::string query="SELECT * FROM "+av.getScope().table()
						+" WHERE "+HIBERLITE_PRIMARY_KEY_COLUMN+"="+Transformer::toSQLiteValue(curId)+";";

	SQLiteSelect sel(av.getConnection(), query);
	if(!sel.step())
		throw database_error("Id " + std::to_string(curId) + " not found in table " + av.getScope().table());
	stmt.push( std::make_pair(sel,curId) );
}

template<class AV>
void LoadBean::notifyDoneWalk(AV& av)
{
	while(!stmt.empty())
		stmt.pop();
}

template<class AV, class E, class S>
void LoadBean::act(AV& av, collection_nvp<E,S> nvp )
{
	av.diveTable(nvp.name);
		std::string tab=av.getScope().table();
		std::vector<sqlid_t> children=Database::dbSelectChildIds(av.getConnection(), tab, stmt.top().second);
		nvp.stream.startLoadFromDb();

		for(size_t i=0;i<children.size();i++){
			E* entry;
			construct(av,&entry,static_cast<unsigned int>(0));

				std::string query="SELECT * FROM "+tab
								+" WHERE "+HIBERLITE_PRIMARY_KEY_COLUMN+"="+Transformer::toSQLiteValue(children[i])+";";
				SQLiteSelect sel(av.getConnection(), query);
				if(!sel.step())
					throw database_error("id not found in the database");
				stmt.push( std::make_pair(sel,children[i]) );

					sql_nvp<E> el("item", *entry);
					av & el;

				stmt.pop();
			nvp.stream.putNext(*entry);
			destroy(*this, entry, static_cast<unsigned int>(0));
		}
	av.pop();
}

template<class C>
C* BeanLoader::loadBean(bean_key key)
{
	C* bean;
	construct(*this, &bean, static_cast<unsigned int>(0));
	startWalk(*bean, key);
	return bean;
}

} //namespace hiberlite

#endif // BEANUPDATER_IMPL_HPP_INCLUDED
