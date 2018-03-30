#ifndef _cGameObject_HG_
#define _cGameObject_HG_

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <string>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <queue>

#include "iDebugRenderer.h"

#include "Physics/cPhysicalProperties.h"
#include "Physics/iPhysicalObject.h"	// For the access to things that will update the positions, etc.

#include "sMeshDrawInfo.h"

class cSimpleAssimpSkinnedMesh;	// Forward declare
class cAnimationState;			// Forward declare 

class cGameObject : public iPhysicalObject
{
public:
	cGameObject();		// constructor
	~cGameObject();		// destructor
//	cGameObject(const cGameObject &obj);  // copy constructor

	//***** from the iPhysicalObject interface ********************
	virtual void SetPhysState( cPhysicalProperties &PhysState );
	virtual cPhysicalProperties GetPhysState( void );
	virtual void GetPhysState( cPhysicalProperties &PhysState );
	//*************************************************************
	glm::vec3 getPosition(void);
	// bOverwritePositionToo effectively stops the object if the "past position" is being used
	void	  overwritePotition( glm::vec3 newPosition, bool bOverwiteOldPositionToo = true );
	glm::quat getQOrientation(void);		// Post-rotation
	void      overwriteQOrientation( glm::quat newOrientation );
	void      overwriteQOrientationEuler( glm::vec3 newOrientationEuler, bool bIsDegrees = true );
	void      adjQOrientation( glm::quat newOrientation );
	void      adjQOrientationEuler( glm::vec3 newOrientationEuler, bool bIsDegrees = true );
	//****************************************************************************************

	//TODO: Add the mediator code
	std::string friendlyName;
	inline unsigned int getUniqueID(void) { return this->m_UniqueID; }

	iDebugRenderer* pDebugRenderer;



	// Mesh information (if drawn)
	// Note: Meshes have a separate orientation and offset from 
	//	the object, in case you want the mesh(es) to be
	//	loaded in different alignment from the game object. 
	//  If the object alignment is the same as the mesh
	//	alignment, then don't set the orientation and offset
	//	in the mesh information.

	std::vector<sMeshDrawInfo> vecMeshes;
	glm::quat getFinalMeshQOrientation(unsigned int meshID);
	glm::quat getFinalMeshQOrientation(glm::quat &meshQOrientation);


	bool bIsVisible;	// If false, any meshes are NOT drawn (not child objects could still be visible)

	// Our "child" objects
	std::vector< cGameObject* > vec_pChildObjects;
	void DeleteChildren(void);
	// Returns NULL if not found
	cGameObject* FindChildByFriendlyName( std::string name );
	cGameObject* FindChildByID( unsigned int ID );


	// Used when there is only one game object (like with text), but we're drawing it many times
	//void pushRenderingState(void);
	//// Ignores call if nothing on stack
	//void popRenderingState(void);

	// If NULL, then object ISN'T a skinned mesh
	cSimpleAssimpSkinnedMesh*	pSimpleSkinnedMesh;	
	cAnimationState*			pAniState;			

private:
	unsigned int m_UniqueID;
	// Used when creating objects
	static unsigned int m_nextUniqueID;

	// All the properties of a physical object 
	cPhysicalProperties m_PhysicalProps;

	//// Any former render states (that have been pushed)
	//struct sRenderState
	//{
	//	glm::vec3 position;
	//	glm::quat oriention;
	//	bool bIsWireframe;
	//	glm::vec4 diffuse;		// Alpha is 4th value
	//	glm::vec4 ambient;
	//	glm::vec3 specular;
	//	float shininess;
	//	glm::vec3 debugColour;
	//	bool bUseDebugColour;
	//	float scale;
	//	bool bIsVisible;
	//};
	//std::queue< sRenderState > m_stackRenderState;

};


#endif
