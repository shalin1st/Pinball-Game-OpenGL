#ifndef _iPersistImp_HG_
#define _iPersistImp_HG_

#include "cPersist.h"

#include <string>

class iPersistImp
{
public:
	virtual ~iPersistImp() {};

	// This is a BS method, but it's to test the lib connection to the SQLite lib
	virtual void SaveSomething(std::string theThing, float theValue) = 0;


};

class cPersistImpFactory
{
public:
	iPersistImp* createPersistImp(cPersist::ePersistType backendDataStore);

};

#endif
