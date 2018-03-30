#ifndef _cPersistImpSQLiteGDPAdapter_HG_
#define _cPersistImpSQLiteGDPAdapter_HG_

#include "../iPersistImp.h"

class cPersistSQLite;

class cPersistImpSQLiteGDPAdapter : public iPersistImp
{
public:
	cPersistImpSQLiteGDPAdapter();
	virtual ~cPersistImpSQLiteGDPAdapter();


	// This is a BS method (to be deleted later)
	virtual void SaveSomething(std::string theThing, float theValue);

private:
	cPersistSQLite* m_pTheSQLite;
};


#endif
