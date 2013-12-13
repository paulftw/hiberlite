#ifndef BeanLoader_H
#define BeanLoader_H

namespace hiberlite{

class LoadBean {
	public:
		template<class AV>
		inline void notifyInitWalk(AV& av);

		template<class AV>
		inline void notifyDoneWalk(AV& av);

		template<class AV, class C>
		inline void act(AV& av, db_atom<C> atom){
			int col_indx=locateCol(av.getScope().prefix());
			atom.loadValue(stmt.top().first, col_indx);
		}

		template<class AV, class E, class S>
		inline void act(AV& av, collection_nvp<E,S> nvp );

	protected:
		std::stack< std::pair<SQLiteSelect,sqlid_t> > stmt;

		inline int locateCol(std::string name){
			SQLiteSelect& sel=stmt.top().first;
			unsigned int n=sel.column_count();
			for(unsigned int i=0;i<n;i++)
			if(sel.get_name(i)==name)
				return i;
			throw database_error(name+": column not found in SELECT query results");
		}
};

class BeanLoader : AVisitor<LoadBean>
{
	public:
		BeanLoader();

		template<class C>
		C* loadBean(bean_key k);

	protected:
		LoadBean actor;

	private:
};

} //namespace hiberlite

#endif // BeanLoader_H
