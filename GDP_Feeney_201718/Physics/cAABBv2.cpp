#include "cAABBv2.h"

// One you WANT to call
cAABBv2::cAABBv2(glm::vec3 minXYZ, float halfSize)
{
	this->minXYZ = minXYZ;
	this->halfSize = halfSize;
	this->genOtherMeasurements();	// min, max, length
	// Calculate and store the ID
	this->m_ID = cAABBv2::calculateID(this->minXYZ);
	return;
}


//cAABBv2::cAABBv2()
//{
//	// AAAAAK!!! 
//	// NEVER CALL THIS
//
//
//}


// Assumes centre and halfSize are already set
void cAABBv2::genOtherMeasurements(void)
{
	this->centre.x = this->minXYZ.x + this->halfSize;
	this->centre.y = this->minXYZ.y + this->halfSize;
	this->centre.z = this->minXYZ.z + this->halfSize;

	this->maxXYZ.x = this->centre.x + this->halfSize;
	this->maxXYZ.y = this->centre.y + this->halfSize;
	this->maxXYZ.z = this->centre.z + this->halfSize;

	this->length = this->halfSize * 2;

	return;
}

// Used to generate the ID (during construction),
//	and also used to calculate ID during lookup
// static so that we can calculate this without making an AABB
// MUST be in the range of -99,999 to 99,999 inclusive
//static 
unsigned long long cAABBv2::calculateID(glm::vec3 locationXYZ)
{
	// 4,000,000,000 approx the largest unsigned 32 bit int
	// 16,000,000,000,000,000,000 approx largest unsigned 64 bit
	// If you do xxxxxx,yyyyyyy,zzzzzz where the xyz are the locations of the AABB, 
	//  AND you ONLY make the AABBs positive, then you have a range of
	//  1,000,000 locations. 
	//	So: 
	//   - from 0 to 999,999   if ALL POSITIVE
	//   - from 0 to  99,999   if positive AND negative

	const unsigned long long Z_MULT = 1;
	const unsigned long long Y_MULT = 1000000;
	// With just the numbers, we get an "integral constant overflow", which I think 
	//	is happening because it's using an 32-bit int as const on the rhs, instead
	//	of a 64 bit int...
	const unsigned long long X_MULT = Y_MULT * Y_MULT;	// 1000000 * 1000000;

	// This gives a C4244 warning, even when using an explicit c-style cast. 
	//	...but it's fine with a C++ static_cast. Seriously? Grrr...
	unsigned long long ID = static_cast<unsigned long long>(
		fabs(floor((double)locationXYZ.x) * (double)X_MULT) +		// ?xxxxx,------,------
		fabs(floor((double)locationXYZ.y) * (double)Y_MULT) +		// ------,?yyyyy,------
		fabs(floor((double)locationXYZ.z))); //* Z_MULT;				// ------,------,?zzzzz

	// You could, if the range isn't large enough:
	//	- recursively nest these AABBs
	//  - Divide the world location by the size....
	//		so (x DIV size), (y DIV size), etc.
	//unsigned long long ID =
	//	(fabs(locationXYZ.x / this->size) * X_MULT) +		// ?xxxxx,------,------
	//	(fabs(locationXYZ.y / this->size) * Y_MULT) +		// ------,?yyyyy,------
	//	(fabs(locationXYZ.z / this->size)); //* Z_MULT;		// ------,------,?zzzzz

					   // Add the "1" values if negative
	const unsigned long long NEGATIVE_SIGN_MULTIPLIER = 100000;	// 100,000

	const unsigned long long NEG_SIGN_FOR_X = X_MULT * NEGATIVE_SIGN_MULTIPLIER;
	const unsigned long long NEG_SIGN_FOR_Y = Y_MULT * NEGATIVE_SIGN_MULTIPLIER;
	const unsigned long long NEG_SIGN_FOR_Z = Z_MULT * NEGATIVE_SIGN_MULTIPLIER;

	if (locationXYZ.x < 0.0f)
	{	// Add 100,000,000,000,000,000
		ID += NEG_SIGN_FOR_X;
	}
	if (locationXYZ.y < 0.0f)
	{	// Add 100,000,000,000
		ID += NEG_SIGN_FOR_Y;
	}
	if (locationXYZ.z < 0.0f)
	{	// Add 100,000
		ID += NEG_SIGN_FOR_Z;
	}

	return ID;
}


//// This is called by the AABBBroadPhase
//bool cAABBv2::isTriListInAABB(cTriForTesttessellated testTriData, bool bStoreIfInside /*=false*/)
//{
//	// TODO: Generate tesselated triangles (to ensure they are inside the AABBs)
//	
////for (int index = 0; index != testTriData.vec_tessellatedTriangles.size(); index++)
////{	
////	// Note, we DON'T want to store the TESSELLATED triangles...
////	if (this->isTriInAABB(testTriData.vec_tessellatedTriangles[index], false))
////	{
////		// At least one triangle is inside this AABB, so store the NON tessellated
////		//	triangle (the larger one)
////		this->vecTriangles.push_back(testTriData.triFromModel);
////		return true;
////	}
////}
//	// If we get here, then NONE of the triangles are inside
//	return false;
//}


// Simple SINGLE triangle test. 
// Called by isTriListInAABB()
//bool cAABBv2::isTriInAABB(sTriAABB testTriData, bool bStoreIfInside /*=false*/)
//{
//	bool isInside = false;
//
//	// Trivial test is if the vertices are inside the AABB. 
//	// If the triangle is "much" smaller than the AABB, this is "good enough". 
//	// However, it's still very possible for an triangle to overlap the AABB, 
//	//	but the VERTICES aren't inside; think: "large" triangle, small AABB.
//	if (this->isPointInAABB(testTriData.verts[0])) 
//	{ 
//		isInside = true;
//	}
//	else if ( this->isPointInAABB(testTriData.verts[1]))	// else if for early exit
//	{ 
//		isInside = true;
//	}
//	else if ( this->isPointInAABB(testTriData.verts[2]))	// else if for early exit
//	{ 
//		isInside = true;
//	}
//	// At this point, the 'trivial' cases are done. 
//	// But the triangle COULD still be intersection with the AABB. 
//	// 
//	// You could also do a triangle-to-AABB test just in case...
//	// - Ericson has some (partial) code in his book
//	// - Tomas Akenine-Möller has:
//	//		- a paper: http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.2.4803&rep=rep1&type=pdf
//	//		- and code: http://fileadmin.cs.lth.se/cs/personal/tomas_akenine-moller/code/tribox3.txt 
//	/// (A key concept of the paper is that if you can find a plane that separates 
//	//	 the triangle from the AABB, then they AREN'T intersecting.)
//	//else if (doMoreComplicatedTriangleAABBIntersectionTest())
//	//{
//	//	isInside = true;
//	//}
//
//	if (isInside && bStoreIfInside)
//	{	// You might want to generate an ID for the triangles
//		//	to see if you have duplicates...
////		this->vecTriangles.push_back(testTriData);
//	}
//	//
//	return isInside;
//}

// Can be called by anything
bool cAABBv2::isPointInAABB(glm::vec3 point)
{
	// Overlap on x?
	if ((point.x >= this->minXYZ.x) && (point.x < this->maxXYZ.x))
	{	// Overlap on y?
		if ((point.y >= this->minXYZ.y) && (point.y < this->maxXYZ.y))
		{	// Overlap on z?
			if ((point.z >= this->minXYZ.z) && (point.z < this->maxXYZ.z))
			{
				return true;
			}
		}
	}
	return false;
}
