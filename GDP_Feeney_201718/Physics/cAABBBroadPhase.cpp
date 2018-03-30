#include "cAABBBroadPhase.h"


#include <algorithm>			// for the std::copy

cAABBBroadPhase::cAABBBroadPhase()
{
	this->pDebugRenderer = NULL;
	return;
}

cAABBBroadPhase::~cAABBBroadPhase()
{
	return;
}

bool cAABBBroadPhase::genAABBGridFromMesh(cMesh &theMesh)
{
	// Assume that the debug renderer is valid
//	this->pDebugRenderer->addTriangle( drTri(glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(2.0f), glm::vec3(1.0f)) );



	return true;
}

bool cAABBBroadPhase::areThereTrianglesInAABB(glm::vec3 testPoint, cAABBv2* &ptheAABB)
{
	// Generate an AABB ID for this test location
	unsigned long long testPointAABBID = cAABBv2::calculateID(testPoint);

	// Find an AABB IF it exists
	std::map< unsigned long long, cAABBv2* >::iterator itAABB = 
				this->mapIDToAABB.find( testPointAABBID );

	// Does it exist? 
	if (itAABB == this->mapIDToAABB.end())
	{
		// NOPE. There are NO AABBs at that location
		return false;
	}
	// Otherwise there IS an AABB
	ptheAABB = itAABB->second;
	return true;
}


