#ifndef _cPersist_HG_
#define _cPersist_HG_

// This is the engine facing pimpl class for the persistence module. 
// The pimpl is used so that we can change the back-end persistence 
//	code. The initial one is using SQLite, simply because that's
//	a way to introduce and-or review basic SQL commands. 
//  (and SQLite is really fast, so there's that)

class iPersistImp;	// Implementation of the pimpl

class cPersist
{
public:
	enum ePersistType
	{
		SQLITE,
		WINAPPDATA,
		JSON		//,
// And whatever other ones you might want...
//		TEXT_TRIVIAL,	
//		TEXT_XML,
//		MAGIC?,
//		DEEP_SPACE_NETWORK?,
//		DIRECT_MENTAL_CONNECTION_TO_THE_HIVE_MIND?
	};
	cPersist(ePersistType backendStorageType);
	~cPersist();


private:
	cPersist();	// Can't call. 
	iPersistImp* m_pImp;

};

#endif
