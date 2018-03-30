#include "cPersistSQLite.h"

#include "sqlite3.h"

#include <iostream>


cPersistSQLite::cPersistSQLite()
{
#ifdef DEBUG
	std::cout << "A cPersistSQLite() is created." << std::endl;
#endif
	return;
}

cPersistSQLite::~cPersistSQLite()
{


	return;
}

