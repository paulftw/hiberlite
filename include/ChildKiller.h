#ifndef CHILDKILLER_H
#define CHILDKILLER_H

namespace hiberlite{

class KillChildren{
	public:
		template<class AV>
		inline void notifyInitWalk(AV& av);

		template<class AV>
		inline void notifyDoneWalk(AV& av);

		template<class AV, class C>
		inline void act(AV& av, db_atom<C> nvp ){}

		template<class AV, class E, class S>
		inline void act(AV& av, collection_nvp<E,S> nvp );

	protected:
		std::stack<sqlid_t> rowId;
};

class ChildKiller : AVisitor<KillChildren>
{
	public:
		ChildKiller();

		template<class C>
		void killChildren(bean_key k, C& bean);

	protected:
		KillChildren actor;

	private:
};

} //namespace hiberlite

#endif // CHILDKILLER_H
