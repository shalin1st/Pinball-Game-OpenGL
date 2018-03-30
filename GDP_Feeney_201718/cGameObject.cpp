#include "cGameObject.h"

#include "iDebugRenderer.h"

// Start the unique IDs at 1. Why not?
/*static*/ unsigned int cGameObject::m_nextUniqueID = 1;

cGameObject::cGameObject()
{
	// Assign a unique ID. 
	// (Be careful of your container calls the default c'tor when copying)
	this->m_UniqueID = cGameObject::m_nextUniqueID++;

	this->pDebugRenderer = NULL;

	this->bIsVisible = false;

	// Set to NULL (or 0, or nullptr)
	this->pSimpleSkinnedMesh = NULL;		// 0
	this->pAniState = NULL;					// 0

	return;
}

cGameObject::~cGameObject()
{
	return;
}


void cGameObject::SetPhysState( cPhysicalProperties &PhysState )
{
	this->m_PhysicalProps = PhysState;
	// cGameObject is friend of cPhysicalProperties, so can set game ID
	this->m_PhysicalProps.m_GameObjectID = this->m_nextUniqueID;
	return;
}

cPhysicalProperties cGameObject::GetPhysState( void )
{
	return this->m_PhysicalProps;
}

void cGameObject::GetPhysState( cPhysicalProperties &PhysState )
{
	PhysState = this->m_PhysicalProps;
	return;
}

glm::vec3 cGameObject::getPosition(void)
{
	return this->m_PhysicalProps.position;
}

void cGameObject::overwritePotition( glm::vec3 newPosition, bool bOverwiteOldPositionToo /*=true*/ )
{
	this->m_PhysicalProps.positionLast = this->m_PhysicalProps.position;
	this->m_PhysicalProps.position = newPosition;
	if ( bOverwiteOldPositionToo )
	{	// Clobber the old position, too (so it looks like we're not moving)
		this->m_PhysicalProps.positionLast = this->m_PhysicalProps.position;
	}
	return;
}

glm::quat cGameObject::getQOrientation(void)
{
	return this->m_PhysicalProps.qOrientation;
}

void cGameObject::overwriteQOrientation( glm::quat newOrientation )
{
	this->m_PhysicalProps.qOrientation = newOrientation;
	return;
}

void cGameObject::overwriteQOrientationEuler( glm::vec3 newOrientationEuler, bool bIsDegrees /*=true*/ )
{
	if ( bIsDegrees )
	{
		newOrientationEuler.x = glm::radians( newOrientationEuler.x );
		newOrientationEuler.y = glm::radians( newOrientationEuler.y );
		newOrientationEuler.z = glm::radians( newOrientationEuler.z );
	}
	this->m_PhysicalProps.qOrientation = glm::quat( newOrientationEuler );
	return;
}

void cGameObject::adjQOrientation( glm::quat newOrientation )
{
	this->m_PhysicalProps.qOrientation *= newOrientation;
	return;
}

void cGameObject::adjQOrientationEuler( glm::vec3 newOrientationEuler, bool bIsDegrees /*= true*/ )
{
	if ( bIsDegrees ) 
	{ 
		newOrientationEuler = glm::vec3( glm::radians(newOrientationEuler.x), 
										 glm::radians(newOrientationEuler.y), 
										 glm::radians(newOrientationEuler.z) ); 
	}

	// Step 1: make a quaternion that represents the angle we want to rotate
	glm::quat rotationAdjust( newOrientationEuler );	
	// Step 2: Multiply this quaternion by the existing quaternion. This "adds" the angle we want.
	this->m_PhysicalProps.qOrientation *= rotationAdjust;
	return;
}

void cGameObject::DeleteChildren(void)
{
	for ( std::vector< cGameObject* >::iterator itChild = this->vec_pChildObjects.begin();
		  itChild != this->vec_pChildObjects.end(); itChild++ )
	{
		// Pointer not zero (0)?
		cGameObject* pTempChildObject = (*itChild);
		if ( pTempChildObject != 0 )
		{
			// Recursively delete all children's children (and so on)
			pTempChildObject->DeleteChildren();
			// Now delete this child
			delete pTempChildObject;
		}
	}
	// There's a vector, but nothing in it
	this->vec_pChildObjects.clear();
	return;
}

cGameObject* cGameObject::FindChildByFriendlyName( std::string name )
{
	for ( std::vector<cGameObject*>::iterator itCGO = this->vec_pChildObjects.begin(); itCGO != this->vec_pChildObjects.end(); itCGO++ )
	{
		if ( (*itCGO)->friendlyName == name )
		{
			return (*itCGO);
		}
	}
	// Didn't find it.
	return NULL;
}

cGameObject* cGameObject::FindChildByID( unsigned int ID )
{
	for ( std::vector<cGameObject*>::iterator itCGO = this->vec_pChildObjects.begin(); itCGO != this->vec_pChildObjects.end(); itCGO++ )
	{
		if ( (*itCGO)->getUniqueID() == ID )
		{
			return (*itCGO);
		}
	}
	// Didn't find it.
	return NULL;
}



//glm::quat cGameObject::getFinalMeshQOrientation(void)
//{
//	return this->m_PhysicalProps.qOrientation * this->m_meshQOrientation;
//}

glm::quat cGameObject::getFinalMeshQOrientation(unsigned int meshID)
{	// Does NOT check for the index of the mesh!
	return this->m_PhysicalProps.qOrientation * this->vecMeshes[meshID].getQOrientation();
}

glm::quat cGameObject::getFinalMeshQOrientation(glm::quat &meshQOrientation)
{	// Does NOT check for the index of the mesh!
	return this->m_PhysicalProps.qOrientation * meshQOrientation;
}
