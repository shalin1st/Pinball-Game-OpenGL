#ifndef _cAABBv2_HG_
#define _cAABBv2_HG_

#include <glm/vec3.hpp>
#include <map>
#include <vector>
#include "AABB_triangles.h"

class cAABBv2
{
public:
	// CAREFUL!! (needed if you store in std containers)
	// Be sure to call genOtherMeasurements() if you 
	//	create default AABB (or copy constructor)
	//cAABBv2();		
	cAABBv2(glm::vec3 minXYZ, float halfSize);	// One you WANT to call
	glm::vec3 centre;
	float halfSize;		// All AABBs are CUBES

						// Assumes centre and halfSize are already set
	void genOtherMeasurements(void);
	float length;
	glm::vec3 minXYZ;
	glm::vec3 maxXYZ;

	// Used to generate the ID (during construction),
	//	and also used to calculate ID during lookup
	// static so that we can calculate this without making an AABB
	// MUST be in the range of -99,999 to 99,999 inclusive
	static unsigned long long calculateID(glm::vec3 locationXYZ);

	// This "inline" is likely OK as it is trivial
	inline unsigned long long getID(void)
	{
		return this->m_ID;
	}

	// This is called by the AABBBroadPhase. If ANY vertex is in the AABB, then tri is stored
	bool isTriInAABB(sTriAABB testTriData, bool bStoreIfInside = false);
	// Passes both the model triangle AND the tessellated list of triangles
	// If any vertex is in the AABB, then triangle is stored
	bool isTriInAABB(cTriForTesttessellated testTriData, bool bStoreIfInside = false);	

	// Can be called by anything
	// If point is between [min, max).
	// (so greater than OR EQUAL to min and 
	//  less than max - but NOT equal)
	// This way, a point that is right on the edge of an AABB is only 
	//	in ONE AABB, rather than many.
	bool isPointInAABB(glm::vec3 point);

	std::vector<sTriAABB> vecTriangles;
private:
	// We don't want to set this after creation,
	//	so is private (if public, we might set it by mistake)
	unsigned long long m_ID;		// unsigned 64 bit
};

#endif 

