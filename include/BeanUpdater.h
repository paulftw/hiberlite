#ifndef BEANUPDATER_H
#define BEANUPDATER_H

#include <stdio.h>
#include <string.h>

namespace hiberlite{

struct RowScope{
	static const int FirstAtom=100;
	std::vector<abstract_atom*> atoms;
	std::string table;
	std::string query;
	sqlid_t id;
	bool needComma;
	template<class C>
	inline void addAssignment(const std::string name, db_atom<C>& atom);

	void addSimpleAssign(const std::string name, std::string value);

	~RowScope();
};

class Transformer{
	public:
		static std::string toSQLiteValue(const double& val){
			char s[64];
			sprintf(s,"%lf",val);
			return s;
		}
		static std::string toSQLiteValue(const float& val){
			char s[64];
			sprintf(s,"%f",val);
			return s;
		}
		static std::string toSQLiteValue(const long double& val){
			char s[64];
			sprintf(s,"%Lf",val);
			return s;
		}

#define HIBERLITE_STRINGIFY_NUM(VALTYPE) \
		static std::string toSQLiteValue(const VALTYPE &val){ \
			char s[64];					\
			char* p=s+63;				\
			*(p--)=0;					\
			VALTYPE tmp=val;			\
			bool needmin=0;				\
			if(tmp<(VALTYPE)0)			\
				needmin=1,tmp*=-1;		\
			do{							\
				*(p--)='0' + (tmp%10);	\
				tmp/=10;				\
			}while(tmp);				\
			if(needmin) *(p--)='-';		\
			return p+1;					\
		}

#define HIBERLITE_STRINGIFY_UNSIGNED_NUM(VALTYPE) \
		static std::string toSQLiteValue(const VALTYPE &val){ \
			char s[64];					\
			char* p=s+63;				\
			*(p--)=0;					\
			VALTYPE tmp=val;			\
			do{							\
				*(p--)='0' + (tmp%10);	\
				tmp/=10;				\
			}while(tmp);				\
			return p+1;					\
		}

HIBERLITE_STRINGIFY_NUM(int)
HIBERLITE_STRINGIFY_UNSIGNED_NUM(unsigned int)
HIBERLITE_STRINGIFY_NUM(char)
//HIBERLITE_STRINGIFY_NUM(unsigned char)
HIBERLITE_STRINGIFY_NUM(long long int)
HIBERLITE_STRINGIFY_UNSIGNED_NUM(unsigned long long int)
#undef HIBERLITE_STRINGIFY_NUM

		static std::string toSQLiteValue(const unsigned char &val){
			char s[64];
			int i=63;
			s[i--]=0;
			unsigned char tmp=val;
			do{
				s[i--]='0' + (tmp%10);
				tmp/=10;
			}while(tmp);
			return s+i+1;
		}


		static std::string toSQLiteValue(const char* val){
			size_t n=strlen(val);
			char* s=new char[4+2*n];
			int x=sprintf(s,"X'");

			for(unsigned int i=0;i<n;i++){
				unsigned char cc=(unsigned char)val[i];
				x+=sprintf(s+x,"%02X", (int)cc);
			}
			sprintf(s+x,"'");
			std::string res(s);
			delete[] s;
			return res;
		}

		static std::string toSQLiteValue(const std::string val){
			return toSQLiteValue(val.c_str());
		}

		static std::string toSQLiteValue(const bool val){
			return val ? "1" : "0";
		}
};

class UpdateBean{
	public:
		template<class AV>
		inline void notifyInitWalk(AV& av);

		template<class AV>
		inline void notifyDoneWalk(AV& av);

		template<class AV, class C>
		inline void act(AV& av, db_atom<C> atom){
			curRow()->addAssignment(av.getScope().prefix(), atom);
		}

		template<class AV, class E, class S>
		inline void act(AV& av, collection_nvp<E,S> nvp );

	protected:
		std::stack< shared_res<RowScope> > rowStack;
		RowScope* curRow();

		void startRow(std::string table, sqlid_t rowid, sqlid_t parent_id, sqlid_t index);
		void commitRow(shared_connection con, sqlid_t rowid);
};

class BeanUpdater : AVisitor<UpdateBean>
{
	public:
		BeanUpdater();

		template<class C>
		void update(bean_key k, C& bean);

	protected:
		UpdateBean actor;

	private:
};

} //namespace hiberlite

#endif // BEANUPDATER_H
