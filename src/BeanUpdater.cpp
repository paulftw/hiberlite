#include "hiberlite.h"

namespace hiberlite{

RowScope::~RowScope()
{
	for(size_t i=0;i<atoms.size();i++)
		delete atoms[i];
}

void RowScope::addSimpleAssign(const std::string name, std::string value){
	if(needComma)
		query+=", ";
	needComma=true;
	query+=name+"="+value;
}

RowScope* UpdateBean::curRow(){
	return rowStack.top().get_object();
}

void UpdateBean::startRow(std::string table, sqlid_t rowid, sqlid_t parent_id, sqlid_t index)
{
	shared_res<RowScope> rs(new RowScope);
	rowStack.push(rs);
	curRow()->query="UPDATE "+table+" SET ";
	curRow()->id=rowid;
	curRow()->needComma=false;
	if(parent_id!=Database::NULL_ID){
		curRow()->addSimpleAssign(HIBERLITE_PARENTID_COLUMN,Transformer::toSQLiteValue(parent_id));
		curRow()->addSimpleAssign(HIBERLITE_ENTRY_INDEX_COLUMN,Transformer::toSQLiteValue(index));
	}
}

void UpdateBean::commitRow(shared_connection con, sqlid_t rowid)
{
	if(!curRow()->needComma){
		rowStack.pop();
		return;
	}
	if(rowid!=curRow()->id)
		throw std::runtime_error("rowid mismatch");
	curRow()->query+=std::string(" WHERE ")+ HIBERLITE_PRIMARY_KEY_COLUMN + "="+ Transformer::toSQLiteValue(rowid) +";";

	sqlite3_stmt* stmt_ptr=NULL;
	{
		sqlite3* db=con->getSQLite3Ptr();
		const char* foob;
		int rc=sqlite3_prepare_v2(db,curRow()->query.c_str(),-1,&stmt_ptr,&foob);
		database_error::database_assert(rc, con);
	}
	shared_stmt statement( new statement_ptr(stmt_ptr) );

	for(size_t i=0;i<curRow()->atoms.size();i++){
		curRow()->atoms[i]->bindValue(statement->get_stmt(), static_cast<int>(i)+RowScope::FirstAtom);
	}

	{
		int rc=sqlite3_step(statement->get_stmt());
		if(rc!=SQLITE_DONE)
			database_error::database_assert(rc, con);
	}


	if(!rowStack.size())
		throw std::logic_error("UpdateVisitor: commit row, but no row started");

	rowStack.pop();
}

BeanUpdater::BeanUpdater() : AVisitor<UpdateBean>(&actor, SAVING)
{}

} //namespace hiberlite
