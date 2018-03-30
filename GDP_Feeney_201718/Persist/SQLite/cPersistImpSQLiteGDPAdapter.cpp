#include "cPersistImpSQLiteGDPAdapter.h"

// Include the library for this
#include "../../../Persist_SQLiteGDPInterface/cPersistSQLite.h"


cPersistImpSQLiteGDPAdapter::cPersistImpSQLiteGDPAdapter()
{
	this->m_pTheSQLite = new cPersistSQLite();
	return;
}

cPersistImpSQLiteGDPAdapter::~cPersistImpSQLiteGDPAdapter()
{
	delete this->m_pTheSQLite;
	return;
}


// This is a BS method (to be deleted later)
void cPersistImpSQLiteGDPAdapter::SaveSomething(std::string theThing, float theValue)
{

	return;
}
