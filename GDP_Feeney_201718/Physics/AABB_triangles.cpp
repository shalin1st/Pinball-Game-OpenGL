#include "AABB_triangles.h"

#include <glm/glm.hpp>

sTriAABB::sTriAABB()
{
	// Clear the vertex values to zeros
	this->verts[0] = this->verts[1] = this->verts[2] = glm::vec3(0.0f);
	this->calculateID(0, 0);
	this->CalcLongestSide();
	return;
}

sTriAABB::sTriAABB( glm::vec3 v0, glm::vec3 v1, glm::vec3 v2 )
{
	this->verts[0] = v0;
	this->verts[1] = v1;
	this->verts[2] = v2;
	this->calculateID( 0, 0 );
	this->CalcLongestSide();
	return;
}


sTriAABB::sTriAABB( glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, unsigned int triID )
{
	this->verts[0] = v0;
	this->verts[1] = v1;
	this->verts[2] = v2;
	this->calculateID( 0, triID );
	this->CalcLongestSide();
	return;
}

sTriAABB::sTriAABB( glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, unsigned int meshID, unsigned int triID )
{
	this->verts[0] = v0;
	this->verts[1] = v1;
	this->verts[2] = v2; 
	this->calculateID(meshID, triID);
	this->CalcLongestSide();
	return;
}


float sTriAABB::CalcLongestSide(void)
{
	float v0v1 = glm::distance( this->verts[0], this->verts[1] );
	float v1v2 = glm::distance( this->verts[1], this->verts[2] );
	float v2v0 = glm::distance( this->verts[2], this->verts[0] );
	
	this->m_longestSideLength = glm::max( v0v1, glm::max( v1v2, v2v0 ) );

	return this->m_longestSideLength;
}

// This is only used to determine if there are duplicate triangles inside the AABBs
// First 9 digits are meshID and next 9 are triID
unsigned long long sTriAABB::calculateID( unsigned long meshID, unsigned long triID )
{
	// m = meshID, t = triID
	// --,mmm,mmm,mmm,ttt,ttt,ttt
	const unsigned long long MESHIDOFFSET = 1000000000;

	this->m_meshID_and_triID = (meshID * MESHIDOFFSET) + triID;

	return this->m_meshID_and_triID;
}

glm::vec3 sTriAABB::calcCentre(void)
{	
	// Centre is average of triangle corners
	return (this->verts[0] + this->verts[1] + this->verts[2]) / 3.0f;
}

glm::vec3 sTriAABB::getCentreEdge_V0toV1(void)
{
	return (this->verts[0] + this->verts[1])/2.0f;
}

glm::vec3 sTriAABB::getCentreEdge_V1toV2(void)
{
	return (this->verts[1] + this->verts[2])/2.0f;
}

glm::vec3 sTriAABB::getCentreEdge_V2toV0(void)
{
	return (this->verts[0] + this->verts[2])/2.0f;
}

// You can pas 0 to 2 into each side
glm::vec3 sTriAABB::getCentreEdge( int vertIndex1, int vertIndex2 )
{
	return (this->verts[vertIndex1] + this->verts[vertIndex2])/2.0f;
}



void cTriForTesttessellated::generateTessellatedTriList( float minSideLength )
{
	// You could recursively perform this, but I'm not doing that.
	// Instead, I'm going to loop over the triangles, and add them 
	//	to the final vector only if they are small enough.

	// Clear the destination triangle list
	this->vec_tessellatedTriangles.clear();

	// Create a temp source triangle list. 
	// This will contain any triangles that are still "too big"
	std::vector<sTriAABB> vecSourceTris;
	// Add the original triangle to this list
	vecSourceTris.push_back( this->triFromModel );

	// Loop through the source list
	for ( std::vector<sTriAABB>::iterator itTri = vecSourceTris.begin();
		  itTri != vecSourceTris.end(); itTri++ )
	{
		// Is the current triangle small enough? 
		if ( itTri->CalcLongestSide() <= minSideLength )
		{	// Yes, so add it to the tessellated vector
			this->vec_tessellatedTriangles.push_back( *itTri );
		}
		else
		{	// Triangle is too big, so split into three triangles
			//
			//                 0
			//                / \
			//               / A \
			// (0-1 split)  X-----X (0-2 split)
			//             / \ D / \
			//            / B \ / C \
			//           1-----X-----2
			//                 |
			//             (1-2 split)
			//
			sTriAABB A, B, C, D;
			
			A.verts[0] = itTri->verts[0];
			A.verts[1] = itTri->getCentreEdge(0,1);
			A.verts[2] = itTri->getCentreEdge(0,2);
			
			B.verts[0] = itTri->getCentreEdge(0,1);
			B.verts[1] = itTri->verts[1];
			B.verts[2] = itTri->getCentreEdge(1,2);
		
			C.verts[0] = itTri->getCentreEdge(0,2);
			C.verts[1] = itTri->getCentreEdge(1,1);
			C.verts[2] = itTri->verts[2];

			D.verts[0] = itTri->getCentreEdge(0,1);
			D.verts[1] = itTri->getCentreEdge(1,2);
			D.verts[2] = itTri->getCentreEdge(0,2);

			// Note we add this to the "too big" triangles, 
			//	but we DON'T bother taking the original one off.
			// Why? Because when we increment the iterator, we will move
			//	to the 'next' triangle, anyway. 
			// Consider: If there was only 1 triangle, and we split it, 
			//	the 1st triangle is the original one, then we add 4 more,
			//	which increases the size of the vector. When we move to 
			//	the 'next' triangle, we will start processing triangle 
			//  "A" of the tessellated triangles. 
			vecSourceTris.push_back(A);
			vecSourceTris.push_back(B);
			vecSourceTris.push_back(C);
			vecSourceTris.push_back(D);
		}// if ( itTri...
	}//for ( std::vector<sTriAABB>...

	return;
}

	
