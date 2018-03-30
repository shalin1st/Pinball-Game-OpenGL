#include "iPersistImp.h"
#include <new>
#include <cassert>	// no stupid C++ 11 static_assert()

// The specific back ends...
#include "SQLite/cPersistImpSQLiteGDPAdapter.h"
#include "JSON/cPersistImpRnRJSON.h"

iPersistImp* cPersistImpFactory::createPersistImp(cPersist::ePersistType backendDataStore)
{
	iPersistImp* pDataStore = nullptr;

	switch (backendDataStore)
	{
	case cPersist::ePersistType::SQLITE:
		pDataStore = new (std::nothrow) cPersistImpSQLiteGDPAdapter();
		break;
	case cPersist::ePersistType::JSON:
		pDataStore = new (std::nothrow) cPersistImpRnRJSON();
		break;
	case cPersist::ePersistType::WINAPPDATA:
		// TODO: This thing
		break;

	default:
		// Seriously, what are you doing with your life? 
		assert("Tried to create a backend persist type that doesn't exist.");
		break;

	}

	return pDataStore;
}
