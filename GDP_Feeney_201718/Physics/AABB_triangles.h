#ifndef _AABB_triangles_HG_
#define _AABB_triangles_HG_

#include <glm/vec3.hpp>
#include <vector>

// These are the triangles that are used for the AABB

// The triangle collision information
// Because there isn't a useful triangle class already 
//	made, here's a specific one for the point-triangle detection
//	for the narrow phase
struct sTriAABB
{
	sTriAABB( glm::vec3 v0, glm::vec3 v1, glm::vec3 v2 );
	sTriAABB( glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, unsigned int triID );
	sTriAABB( glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, unsigned int meshID, unsigned int triID );
	sTriAABB();
	glm::vec3 verts[3];		// 3 corners of the triangle
	// This is only used to determine if there are duplicate triangles inside the AABBs
	// First 9 digits are meshID and next 9 are triID
	unsigned long long calculateID( unsigned long meshID, unsigned long triID );
	inline unsigned long long getID(void)
	{
		return this->m_meshID_and_triID;
	}
	// TODO: Add sexy math code to do this...
	float CalcLongestSide(void);
	inline float getLongestSide(void)
	{
		return this->m_longestSideLength;
	}
	glm::vec3 calcCentre(void);
	glm::vec3 getCentreEdge_V0toV1(void);
	glm::vec3 getCentreEdge_V1toV2(void);
	glm::vec3 getCentreEdge_V2toV0(void);
	// You can pas 0 to 2 into each side
	glm::vec3 getCentreEdge( int vertIndex1, int vertIndex2 );
private:
	float m_longestSideLength;
	unsigned long long m_meshID_and_triID;
};

// This represents each triangle from the model 
//	(and the actual triangles we are using to generate the AABBs)
// Could either:
// - if nothing is in the vector, then use the actual model triangle for testing
// - or place the actual triangle in the test vector, then only test the vector
// BUT you STORE the model triangle, NOT the tessellated triangles into the AABBs
class cTriForTesttessellated
{
public:
	sTriAABB triFromModel;				// Ply triangle
	// Check if this triangle (from the model) is LTE one half the AABB size
	// If it IS, then subdivide and add to this vector.
	// Subdivide by splitting the edges of the triangle, generating 4 smaller ones
	// (Note: if you split by the centre, you get smaller triangles in area,
	//  but the longest edges remain the same, so don't do that.)
	// - Continue recursively until it's LTE one the AABB size
	void generateTessellatedTriList( float minSideLength );
	std::vector< sTriAABB > vec_tessellatedTriangles;
};

#endif
