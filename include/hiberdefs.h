#ifndef HIBERDEFS_H_INCLUDED
#define HIBERDEFS_H_INCLUDED

#define HIBERLITE_NVP(Field) hiberlite::sql_nvp< typeof(Field) >(#Field,Field)
#define HIBERLITE_BASE_CLASS(ClName) hiberlite::sql_nvp< ClName >(#ClName,*((ClName*)this) )

#define HIBERLITE_EXPORT_CLASS(ClName) 			\
namespace hiberlite{							\
template<>										\
std::string Database::getClassName<ClName>()	\
{	return #ClName;	}}

//#define HIBERLITE_COLLECTION(Field) hiberlite::collection_nvp<typeof(Field),typeof(Field.begin())>(#Field, Field, Field.begin(), Field.end())
//#define HIBERLITE_ARRAY(Field,N) hiberlite::collection_nvp<typeof(Field),typeof(Field[0])>(#Field,  *Field, *(Field+N))

#define HIBERLITE_PRIMARY_KEY_COLUMN "hiberlite_id"
#define HIBERLITE_PARENTID_COLUMN "hiberlite_parent_id"
#define HIBERLITE_ENTRY_INDEX_COLUMN "hiberlite_entry_indx"
#define HIBERLITE_ID_STORAGE_CLASS "INTEGER"

#define HIBERLITE_PRIMARY_KEY_STORAGE_TYPE "PRIMARYKEY"

#endif // HIBERDEFS_H_INCLUDED
