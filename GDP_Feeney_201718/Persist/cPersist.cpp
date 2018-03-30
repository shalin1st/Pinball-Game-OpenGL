#include <assert.h>
#include "cPersist.h"

// Note: pimpl has actual implementation included HERE, 
//	in the body, NOT in the header
#include "iPersistImp.h"

cPersist::cPersist(ePersistType backendStorageType)
{
	cPersistImpFactory PF;

	// Note that the factory won't throw an exception 
	this->m_pImp = PF.createPersistImp(backendStorageType);

	return;
}


cPersist::~cPersist()
{
	return;
}

// Can't call. 
cPersist::cPersist()
{
	assert("cPersist() created without a back-end store (default constructor was called)");
	return;
}
