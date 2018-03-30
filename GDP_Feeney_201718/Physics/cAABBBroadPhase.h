#ifndef _cAABBBroadPhase_HG_
#define _cAABBBroadPhase_HG_

#include "cAABBv2.h"
#include "../cMesh.h"
#include "../iDebugRenderer.h"


class cAABBBroadPhase
{
public:
	cAABBBroadPhase();
	~cAABBBroadPhase();

	bool genAABBGridFromMesh(cMesh &theMesh);


	// Call this method to see what triangles MIGHT hit this test point
	bool areThereTrianglesInAABB(glm::vec3 testPoint, cAABBv2* &ptheAABB);
//	bool getVecTrianglesInAABB(glm::vec3 testPoint, std::vector<sTriAABB> &vecTris);

	// Contained AABBs
	std::map< unsigned long long, cAABBv2* > mapIDToAABB;

	iDebugRenderer* pDebugRenderer;
};

#endif
