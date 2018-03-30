#ifndef _ccPersistImpRnRJSON_HG_
#define _cPersistImpRnRJSON_HG_

#include "../iPersistImp.h"

class cPersistImpRnRJSON : public iPersistImp
{
public:
	cPersistImpRnRJSON();
	virtual ~cPersistImpRnRJSON();


	// This is a BS method (to be deleted later)
	virtual void SaveSomething(std::string theThing, float theValue);


};



#endif
