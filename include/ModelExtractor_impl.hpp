#ifndef MODELEXTRACTOR_IMPL_HPP_INCLUDED
#define MODELEXTRACTOR_IMPL_HPP_INCLUDED

namespace hiberlite{

template<class AV, class E, class S>
void ExtractModel::act(AV& av, collection_nvp<E,S> nvp )
{
	E* entry;
	construct(av, &entry, static_cast<unsigned int>(0));
		av.diveTable(nvp.name);

			putNamedColumn(av.getScope().table(), HIBERLITE_PARENTID_COLUMN, HIBERLITE_ID_STORAGE_CLASS);
			putNamedColumn(av.getScope().table(), HIBERLITE_ENTRY_INDEX_COLUMN, HIBERLITE_ID_STORAGE_CLASS);

			sql_nvp<E> el("item",*entry);
			av & el;
		av.pop();
	destroy(*this, entry, static_cast<unsigned int>(0));
}

template<class C>
void ModelExtractor::extractModel(shared_connection _con)
{
	C* bean;
	construct(*this, &bean, static_cast<unsigned int>(0));
	bean_key key(_con, Database::NULL_ID);
	startWalk(*bean, key);
	destroy(*this, bean, static_cast<unsigned int>(0));
}

} //namespace hiberlite

#endif // MODELEXTRACTOR_IMPL_HPP_INCLUDED
