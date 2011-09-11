#include "hiberlite.h"

namespace hiberlite{

void Model::add(Table t)
{
	if( find(t.name)!=end() )
		throw std::logic_error("table ["+t.name+"] already exists");
	HIBERLITE_HL_DBG_DO( std::cout << "model add table " << t.name << std::endl; )
	insert( std::pair<std::string,Table>(t.name,t) );
}

Table::Table()
{
	add(Column(HIBERLITE_PRIMARY_KEY_COLUMN,HIBERLITE_PRIMARY_KEY_STORAGE_TYPE));
}

void Table::add(Column c)
{
	HIBERLITE_HL_DBG_DO( std::cout << "table " << name << " add column " << c.name << " : " << c.storage_type << std::endl; )
	columns[c.name]=c;
}

bool Table::contains(std::string colname)
{
	return columns.find(colname)!=columns.end();
}

std::string Scope::name_column(std::string name)
{
	if(prefix().size())
		return prefix()+"_"+name;
	else
		return name;
}

std::string Scope::full()
{
	if(prefix().size())
		return table()+"_"+prefix();
	else
		return table();
}

} //namespace hiberlite
