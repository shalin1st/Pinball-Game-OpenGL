#ifndef _cPhysicsWorld_HG_
#define _cPhysicsWorld_HG_

#include "cAABBBroadPhase.h"

class cPhysicsWorld
{
public:
	cPhysicsWorld();
	~cPhysicsWorld();

	void IntegrationStep( double deltaTime );



};

#endif
