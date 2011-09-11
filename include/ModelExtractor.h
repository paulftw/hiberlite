#ifndef MODELEXTRACTOR_H
#define MODELEXTRACTOR_H

namespace hiberlite{

class ExtractModel{
	public:
		Model getModel();

		template<class AV>
		inline void notifyInitWalk(AV& av){
			model[av.getScope().table()].name=av.getScope().table();
		}

		template<class AV>
		inline void notifyDoneWalk(AV& av){}

		template<class AV, class X>
		inline void act(AV& av, db_atom<X> atom) {
			putColumn(av, atom.sqliteStorageClass());
		}

		template<class AV, class E, class S>
		inline void act(AV& av, collection_nvp<E,S> nvp );

	protected:
		template<class AV>
		inline void putColumn(AV& av, std::string storage)
		{
			Scope scope=av.getScope();
			putNamedColumn(scope.table(), scope.prefix(), storage);
		}

		inline void putNamedColumn(std::string table, std::string colname, std::string storClass){
			model[ table ].name=table;
			if( model[table].contains( colname ) )
				throw std::logic_error(colname+": column name is in use");
			model[ table ].add( Column(colname,storClass) );
		}

		Model model;
};

class ModelExtractor : AVisitor<ExtractModel>{
	public:
		ModelExtractor();
		template<class C>
		void extractModel(shared_connection _con);
		Model getModel();

	protected:
		ExtractModel actor;
};

} //namespace hiberlite

#endif // MODELEXTRACTOR_H
