// This file is used to laod the models
#include "cGameObject.h"
#include <vector>
#include "Utilities.h"		// getRandInRange()
#include <glm/glm.hpp>
#include "globalGameStuff.h"

// For the cSimpleAssimpSkinnedMeshLoader class
#include "assimp/cSimpleAssimpSkinnedMeshLoader_OneMesh.h"

#include "cAnimationState.h"

extern std::vector< cGameObject* >  g_vecGameObjects;
extern cGameObject* g_pTheDebugSphere;
// Used for the light pass of the deferred rendering
extern cGameObject* g_pLightSphere2Sided;

extern cGameObject* g_ExampleTexturedQuad;


//const float SURFACEOFGROUND = -10.0f;
//const float RIGHTSIDEWALL = 15.0f;
//const float LEFTSIDEWALL = -15.0f;



void LoadModelsIntoScene(void)
{

	//for ( int count = 0; count != 1; count++ )
	//{
	//	{// Dalek
	//		cGameObject* pDalek = new cGameObject();
	//		pDalek->friendlyName = "Big D";
	//		cPhysicalProperties physState;
	//		physState.position = glm::vec3(getRandInRange(-100,100), 0.0, getRandInRange(-100, 100));
	//		physState.setOrientationEulerAngles(glm::vec3(0.0, 0.0, 0.0f));
	//		pDalek->SetPhysState(physState);
	//		sMeshDrawInfo meshInfo;
	//		meshInfo.scale = 1.0;
	//		meshInfo.name = "dalek2005_xyz_uv_res_2.ply";
	//		meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("GuysOnSharkUnicorn.bmp", 0.0f));
	//		meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("Utah_Teapot_xyz_n_uv_Enterprise.bmp", 1.0f));
	//		meshInfo.setMeshOrientationEulerAngles(glm::vec3(-90.0f,0.0f,0.0f), true);
	//		pDalek->vecMeshes.push_back(meshInfo);
	//		::g_vecGameObjects.push_back(pDalek);
	//	}
	//}

	{// Room model (for stencil buffer example)
		::g_Room = new cGameObject();
		::g_Room->friendlyName = "Room";
		cPhysicalProperties physState;
		physState.position = glm::vec3(0.0f, 0.0, 0.0f);
		physState.setOrientationEulerAngles(glm::vec3(0.0, 0.0, 0.0f));
		::g_Room->SetPhysState(physState);
		sMeshDrawInfo meshInfo;
		meshInfo.scale = 50.0;
//		meshInfo.debugDiffuseColour = glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f );
//		meshInfo.bDrawAsWireFrame = true;
//		meshInfo.bUseDebugColour = true;
//		meshInfo.debugDiffuseColour = glm::vec4( 0.0f, 1.0f, 1.0f, 1.0f );
		meshInfo.name = "Room_2_Bigger_Triangulated.ply";
		meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("GuysOnSharkUnicorn.bmp", 0.0f));
		meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("Utah_Teapot_xyz_n_uv_Enterprise.bmp", 1.0f));
		::g_Room->vecMeshes.push_back(meshInfo);
	}
	{// Stencil mask (matches frame for doorway in Room model)
		::g_RoomMaskForStencil = new cGameObject();
		::g_RoomMaskForStencil->friendlyName = "RoomDoorMask";
		cPhysicalProperties physState;
		physState.position = glm::vec3(0.0f, 0.0, 0.0f);
		physState.setOrientationEulerAngles(glm::vec3(0.0f, 0.0, 0.0f));
		::g_RoomMaskForStencil->SetPhysState(physState);
		sMeshDrawInfo meshInfo;
		meshInfo.scale = 50.0;
		//meshInfo.bUseDebugColour = true;
		//meshInfo.debugDiffuseColour = glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f );
		//meshInfo.bDrawAsWireFrame = true;
		meshInfo.name = "Just_Inside_Door_Frame_for_Masking.ply";
		meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("GuysOnSharkUnicorn.bmp", 1.0f));
		meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("Utah_Teapot_xyz_n_uv_Enterprise.bmp", 0.0f));
		::g_RoomMaskForStencil->vecMeshes.push_back(meshInfo);
	}
	
			
			
			
	{// 2 sided quad
		::g_ExampleTexturedQuad = new cGameObject();
		::g_ExampleTexturedQuad->friendlyName = "quad";
		cPhysicalProperties physState;
		physState.position = glm::vec3(100.0f, 100.0, -250.0f);
		physState.setOrientationEulerAngles(glm::vec3(90.0f, 0.0, 0.0f));
		::g_ExampleTexturedQuad->SetPhysState(physState);
		sMeshDrawInfo meshInfo;
		meshInfo.scale = 75;
		meshInfo.setMeshOrientationEulerAngles(glm::vec3(0.0f, 0.0f, 0.0f)); 
//		meshInfo.debugDiffuseColour = glm::vec4( 0.0f, 1.0f, 0.0f, 1.0f );
//		meshInfo.bDrawAsWireFrame = true;
		meshInfo.name = "2SidedQuad";
		meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("GuysOnSharkUnicorn.bmp", 0.0f));
		meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("Utah_Teapot_xyz_n_uv_Enterprise.bmp", 1.0f));
		::g_ExampleTexturedQuad->vecMeshes.push_back(meshInfo);
		//::g_vecGameObjects.push_back( pTempGO );		// Fastest way to add
	}		

	{	// Skinned mesh  model
		cGameObject* pTempGO = new cGameObject();
		pTempGO->friendlyName = "Justin";
		// This assigns the game object to the particular skinned mesh type 
		pTempGO->pSimpleSkinnedMesh = ::g_pRPGSkinnedMesh;
		// Add a default animation 
		pTempGO->pAniState = new cAnimationState();
		pTempGO->pAniState->defaultAnimation.name = "assets/modelsFBX/RPG-Character_Unarmed-Walk(FBX2013).FBX";
		pTempGO->pAniState->defaultAnimation.frameStepTime = 0.005f;
		// Get the total time of the entire animation
		pTempGO->pAniState->defaultAnimation.totalTime = pTempGO->pSimpleSkinnedMesh->GetDuration();

		cPhysicalProperties physState;
		physState.position = glm::vec3(-50.0f, 0.0, 0.0f);
		pTempGO->SetPhysState(physState);
		sMeshDrawInfo meshInfo;
		meshInfo.scale = 0.25f;
		meshInfo.setMeshOrientationEulerAngles(glm::vec3(0.0f, 0.0f, 0.0f)); 
		meshInfo.debugDiffuseColour = glm::vec4( 1.0f, 1.0f, 0.0f, 1.0f );
		meshInfo.bDrawAsWireFrame = true;
		meshInfo.name = ::g_pRPGSkinnedMesh->friendlyName;
		pTempGO->vecMeshes.push_back(meshInfo);
		::g_vecGameObjects.push_back( pTempGO );		// Fastest way to add
	}	
	{	// Skinned mesh  model
		cGameObject* pTempGO = new cGameObject();
		pTempGO->friendlyName = "Sophie";
		// This assigns the game object to the particular skinned mesh type 
		pTempGO->pSimpleSkinnedMesh = ::g_pRPGSkinnedMesh;
		// Add a default animation 
		pTempGO->pAniState = new cAnimationState();
		pTempGO->pAniState->defaultAnimation.name = "assets/modelsFBX/RPG-Character_Unarmed-Walk(FBX2013).FBX";
		pTempGO->pAniState->defaultAnimation.frameStepTime = 0.01f;
		// Get the total time of the entire animation
		pTempGO->pAniState->defaultAnimation.totalTime = pTempGO->pSimpleSkinnedMesh->GetDuration();

		{
			cAnimationState::sStateDetails leftAnimation;
			leftAnimation.name = "assets/modelsFBX/RPG-Character_Unarmed-Roll-Left(FBX2013).FBX";
			//leftAnimation.totalTime = pTempGO->pSimpleSkinnedMesh->FindAnimationTotalTime(leftAnimation.name);
			leftAnimation.totalTime = 0.1f;
			leftAnimation.frameStepTime = 0.001f;

			cAnimationState::sStateDetails rightAnimation;
			rightAnimation.name = "assets/modelsFBX/RPG-Character_Unarmed-Roll-Right(FBX2013).FBX";
			//rightAnimation.totalTime = pTempGO->pSimpleSkinnedMesh->FindAnimationTotalTime(rightAnimation.name);
			rightAnimation.totalTime = 1.0f;
			rightAnimation.frameStepTime = 0.001f;

			cAnimationState::sStateDetails jumpAnimation;
			jumpAnimation.name = "assets/modelsFBX/RPG-Character_Unarmed-Jump(FBX2013).FBX";
			jumpAnimation.totalTime = pTempGO->pSimpleSkinnedMesh->FindAnimationTotalTime(jumpAnimation.name);
			jumpAnimation.totalTime = 1.0f;
			jumpAnimation.frameStepTime = 0.001f;

//			pTempGO->pAniState->vecAnimationQueue.push_back( leftAnimation );
//			pTempGO->pAniState->vecAnimationQueue.push_back( rightAnimation );
//			pTempGO->pAniState->vecAnimationQueue.push_back( jumpAnimation );
//			pTempGO->pAniState->vecAnimationQueue.push_back( rightAnimation );
//			pTempGO->pAniState->vecAnimationQueue.push_back( leftAnimation );
//			pTempGO->pAniState->vecAnimationQueue.push_back( jumpAnimation );
//			pTempGO->pAniState->vecAnimationQueue.push_back( jumpAnimation );

		}

		cPhysicalProperties physState;
		physState.position = glm::vec3(+50.0f, 0.0, 0.0f);
		pTempGO->SetPhysState(physState);
		sMeshDrawInfo meshInfo;
		meshInfo.scale = 0.25f;
		meshInfo.setMeshOrientationEulerAngles(glm::vec3(0.0f, 0.0f, 0.0f)); 
		meshInfo.debugDiffuseColour = glm::vec4( 1.0f, 1.0f, 0.0f, 1.0f );
		meshInfo.bDrawAsWireFrame = true;
		meshInfo.name = ::g_pRPGSkinnedMesh->friendlyName;
		pTempGO->vecMeshes.push_back(meshInfo);
		::g_vecGameObjects.push_back( pTempGO );		// Fastest way to add
	}	

	//for ( float x = -100.0f; x < 100.0f; x += 10.0f )
	//{
	//	for ( float z = -100.0f; z < 100.0f; z += 10.0f )

	//	{	// Skinned mesh  model
	//	cGameObject* pTempGO = new cGameObject();
	//	pTempGO->friendlyName = "Sophie";
	//	// This assigns the game object to the particular skinned mesh type 
	//	pTempGO->pSimpleSkinnedMesh = ::g_pRPGSkinnedMesh;
	//	cPhysicalProperties physState;
	//	physState.position = glm::vec3(x, 0.0, z);
	//	pTempGO->SetPhysState(physState);
	//	sMeshDrawInfo meshInfo;
	//	meshInfo.scale = 0.25f;
	//	meshInfo.setMeshOrientationEulerAngles(glm::vec3(0.0f, 0.0f, 0.0f)); 
	//	meshInfo.debugDiffuseColour = glm::vec4( 1.0f, 1.0f, 0.0f, 1.0f );
	//	meshInfo.bDrawAsWireFrame = true;
	//	meshInfo.name = ::g_pRPGSkinnedMesh->friendlyName;
	//	pTempGO->vecMeshes.push_back(meshInfo);
	//	::g_vecGameObjects.push_back( pTempGO );		// Fastest way to add
	//	}	
	//}

	{
		cGameObject* pTempGO = new cGameObject();
		cPhysicalProperties physState;
		pTempGO->friendlyName = "NCC-1701";
		physState.integrationUpdateType = cPhysicalProperties::EXCLUDED_FROM_INTEGRATION;
		physState.mass = physState.inverseMass = 0.0f;	// Infinite
		physState.position.z = -350.0f;
		pTempGO->SetPhysState(physState);

		sMeshDrawInfo meshInfo;
		meshInfo.scale = 1.0f;
		meshInfo.debugDiffuseColour = glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f );
		meshInfo.name = "Ship_Pack_WIP_mod - command_xyz_n_uv.obj";
		//meshInfo.debugDiffuseColour = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		//meshInfo.bUseDebugColour = true;
		//meshInfo.bDrawAsWireFrame = true;
		meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("GuysOnSharkUnicorn.bmp", 1.0f));
		meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("Utah_Teapot_xyz_n_uv_Enterprise.bmp", 0.0f));
		pTempGO->vecMeshes.push_back(meshInfo);

		::g_vecGameObjects.push_back( pTempGO );		// Fastest way to add
	}	
	
	
	{
		cGameObject* pTempGO = new cGameObject();
		cPhysicalProperties physState;
		physState.integrationUpdateType = cPhysicalProperties::EXCLUDED_FROM_INTEGRATION;
		physState.mass = physState.inverseMass = 0.0f;	// Infinite
		physState.position.x = 75.0f;
		physState.position.y = 25.0f;
//		physState.position.y = -100.0f;
		physState.setRotationalSpeedEuler( glm::vec3(0.0f, 0.5f, 0.0f) );
		pTempGO->SetPhysState(physState);


		sMeshDrawInfo meshInfo;
		meshInfo.debugDiffuseColour = glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f );
		meshInfo.scale = 350.0f;
		meshInfo.name = "bunny";	

		//meshInfo.scale = 0.65f;
		//meshInfo.name = "teapotUV";	

		// Make bunny reflective...
		meshInfo.bIsEnvirMapped = true;
		meshInfo.reflectBlendRatio = 0.5f;
		meshInfo.refractBlendRatio = 0.5f;
		meshInfo.coefficientRefract = 0.1f;

		// 
		pTempGO->friendlyName = "bugs";

		meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("barberton_etm_2001121_lrg.bmp", 1.0f));
		meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("height_map_norway-height-map-aster-30m.bmp", 0.0f));
		pTempGO->vecMeshes.push_back(meshInfo);

		::g_vecGameObjects.push_back( pTempGO );		// Fastest way to add
	}	
	{
		cGameObject* pTempGO = new cGameObject();
		cPhysicalProperties physState;
		physState.integrationUpdateType = cPhysicalProperties::EXCLUDED_FROM_INTEGRATION;
		physState.mass = physState.inverseMass = 0.0f;	// Infinite
		physState.position.y = -100.0f;
		pTempGO->SetPhysState(physState);

		sMeshDrawInfo meshInfo;
		meshInfo.scale = 1.0f;
		meshInfo.debugDiffuseColour = glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f );
		meshInfo.name = "MeshLabTerrain_xyz_n_uv";	
		meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("GuysOnSharkUnicorn.bmp", 1.0f));
		meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("Utah_Teapot_xyz_n_uv_Enterprise.bmp", 0.0f));
		pTempGO->vecMeshes.push_back(meshInfo);

		::g_vecGameObjects.push_back( pTempGO );		// Fastest way to add
	}
	//{	// Right side plane
	//	cGameObject* pTempGO = new cGameObject();
	//	pTempGO->position.x = 15.0f;	// RIGHTSIDEWALL;		// Where our physics HACK detection is
	//	pTempGO->orientation.z = glm::radians(0.0f);	// Degrees
	//	pTempGO->orientation2.z = glm::radians(90.0f);	// Degrees   
	//	pTempGO->scale = 1.0f;
	//	pTempGO->diffuseColour = glm::vec4( 0.0f, 1.0f, 1.0f, 1.0f );
	//	pTempGO->meshName = "PlaneXZ";		// Was teapot
	//	pTempGO->typeOfObject = eTypeOfObject::PLANE;
	//	// ***
	//	pTempGO->bIsUpdatedInPhysics = false;	
	//	// ***
	//	::g_vecGameObjects.push_back( pTempGO );		// Fastest way to add
	//}

	{
		cGameObject* pTempGO = new cGameObject();
		cPhysicalProperties physState;
		physState.position = glm::vec3(-4.0f, 4.0f, 0.0f);
		physState.velocity = glm::vec3(2.0f, 1.0f, 0.0f);
		physState.setOrientationEulerAngles(glm::vec3(0.0f, 0.0f, 0.0f));
		physState.rigidBodyShape = cPhysicalProperties::SPHERE;
		physState.rigidBodyMeasurements.sphere_capsule_radius = 1.0f;
		pTempGO->SetPhysState(physState);
		sMeshDrawInfo meshInfo;
		meshInfo.scale = 1.0f;
		meshInfo.debugDiffuseColour = glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f );
		meshInfo.name = "SphereRadius1";
		pTempGO->vecMeshes.push_back(meshInfo);
		//
		::g_vecGameObjects.push_back( pTempGO );		// Fastest way to add
	}	
	{
		cGameObject* pTempGO = new cGameObject();
		cPhysicalProperties physState;
		physState.position = glm::vec3(-0.5f, 4.0f, 0.0f);
		physState.rigidBodyShape = cPhysicalProperties::SPHERE;
		physState.rigidBodyMeasurements.sphere_capsule_radius = 1.0f;
		pTempGO->SetPhysState(physState);
		sMeshDrawInfo meshInfo;
		meshInfo.scale = 1.0f;
		meshInfo.debugDiffuseColour = glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f );
		meshInfo.name = "SphereRadius1";			// was dolphin
		pTempGO->vecMeshes.push_back(meshInfo);
		::g_vecGameObjects.push_back( pTempGO );		// Fastest way to add
	}
	// Add a bunch of spheres....
	for ( int count = 0; count != 20; count++ )
	{
		cGameObject* pTempGO = new cGameObject();
		cPhysicalProperties physState;
		physState.position.x = getRandInRange<float>(-7.0f, 7.0f);	
		physState.position.y = getRandInRange<float>( 1.0f, 10.0f ); 
		physState.velocity.x = getRandInRange<float>( -3.0f, 3.0f );
		physState.velocity.y = getRandInRange<float>( -1.0f, 2.0f );
		physState.rigidBodyShape = cPhysicalProperties::SPHERE;
		physState.rigidBodyMeasurements.sphere_capsule_radius = 1.0f;
		pTempGO->SetPhysState(physState);
		sMeshDrawInfo meshInfo;
		meshInfo.scale = 1.0f;
		meshInfo.debugDiffuseColour = glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f );
		meshInfo.name = "SphereRadius1";
		pTempGO->vecMeshes.push_back(meshInfo);
		//
		::g_vecGameObjects.push_back( pTempGO );		// Fastest way to add
	}// for ( int count...

	// Teapots at the bottom of vector, so are drawn last...
	{	// Teapot #1
		cGameObject* pTempGO = new cGameObject();
		pTempGO->friendlyName = LEFTTEAPOTNAME;
		cPhysicalProperties physState;
		physState.position.x = 4.0f;
		physState.integrationUpdateType = cPhysicalProperties::ePhysicsIntegrationUpdateTypes::EXCLUDED_FROM_INTEGRATION;
		pTempGO->SetPhysState(physState);
		sMeshDrawInfo meshInfo;
		meshInfo.scale = 0.1f;
		meshInfo.name = "teapotUV";		
		meshInfo.globalAlpha = 0.75f;
		meshInfo.vecMehs2DTextures.push_back( sTextureBindBlendInfo("Utah_Teapot_xyz_n_uv_Enterprise.bmp", 0.0f) );
		meshInfo.vecMehs2DTextures.push_back( sTextureBindBlendInfo("GuysOnSharkUnicorn.bmp", 1.0f) );
		pTempGO->vecMeshes.push_back(meshInfo);
		//
		::g_vecGameObjects.push_back(pTempGO);		// Fastest way to add
	}
	{	// Teapot #2
		cGameObject* pTempGO = new cGameObject();
		pTempGO->friendlyName = "Right Teapot";
		cPhysicalProperties physState;
		physState.position.x = -4.0f;
		physState.integrationUpdateType = cPhysicalProperties::ePhysicsIntegrationUpdateTypes::EXCLUDED_FROM_INTEGRATION;
		pTempGO->SetPhysState(physState);
		sMeshDrawInfo meshInfo;
		meshInfo.scale = 0.1f;
		meshInfo.name = "teapotUV";		
		meshInfo.globalAlpha = 0.75f;
		meshInfo.vecMehs2DTextures.push_back( sTextureBindBlendInfo("Utah_Teapot_xyz_n_uv_Enterprise.bmp", 0.5f) );
		meshInfo.vecMehs2DTextures.push_back( sTextureBindBlendInfo("GuysOnSharkUnicorn.bmp", 0.5f) );
		pTempGO->vecMeshes.push_back(meshInfo);
		//
		::g_vecGameObjects.push_back(pTempGO);		// Fastest way to add

	}

	// Our skybox object
	{
		//cGameObject* pTempGO = new cGameObject();
		::g_pSkyBoxObject = new cGameObject();
		cPhysicalProperties physState;
		::g_pSkyBoxObject->SetPhysState(physState);
		sMeshDrawInfo meshInfo;
		meshInfo.scale = 10000.0f;
		meshInfo.debugDiffuseColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		meshInfo.name = "SmoothSphereRadius1InvertedNormals";			
		meshInfo.vecMeshCubeMaps.push_back(sTextureBindBlendInfo("space", 1.0f));
		meshInfo.bIsSkyBoxObject = true;
		::g_pSkyBoxObject->vecMeshes.push_back(meshInfo);
		// IS SKYBOX
		::g_vecGameObjects.push_back(::g_pSkyBoxObject);		// Fastest way to add
	}


	//// HACK: Set the objects to random colours
	//for ( int index = 0; index != ::g_vecGameObjects.size(); index++ )
	//{
	//	::g_vecGameObjects[index]->diffuseColour.r = getRandInRange(0.0f, 1.0f);
	//	::g_vecGameObjects[index]->diffuseColour.g = getRandInRange(0.0f, 1.0f);
	//	::g_vecGameObjects[index]->diffuseColour.b = getRandInRange(0.0f, 1.0f);
	//}

	//// HACK: Set texture values to something if there isn't a name
	//for (int index = 0; index != ::g_vecGameObjects.size(); index++)
	//{
	//	if (::g_vecGameObjects[index]->textureNames[0] == "")
	//	{
	//		::g_vecGameObjects[index]->textureNames[0] = "Utah_Teapot_xyz_n_uv_Enterprise.bmp";
	//		::g_vecGameObjects[index]->textureNames[1] = "GuysOnSharkUnicorn.bmp";
	//		::g_vecGameObjects[index]->textureBlend[0] = getRandInRange(0.0f, 1.0f);
	//		::g_vecGameObjects[index]->textureBlend[1] = 1.0f - ::g_vecGameObjects[index]->textureBlend[0];
	//	}
	//}//for (int index = 0; index 



//	// Add a bunch more rabbits
//	const float SIZEOFWORLD = 6.0f;	
////	for ( int index = 3; index < MAXNUMBEROFGAMEOBJECTS; index++ )
//	for ( int index = 3; index < 100; index++ )
//	{
//		cGameObject* pTempGO = new cGameObject();
//		pTempGO->position.x = getRandInRange<float>(-SIZEOFWORLD, SIZEOFWORLD );
//		pTempGO->position.y = getRandInRange<float>(-SIZEOFWORLD, SIZEOFWORLD );
//		pTempGO->position.z = getRandInRange<float>(-SIZEOFWORLD, SIZEOFWORLD );
//		//::g_GameObjects[index]->scale = getRandInRange<float>( 7.0f, 15.0f );
//		// Pick a random colour for this bunny
//		pTempGO->diffuseColour.r = getRandInRange<float>(0.0f, 1.0f );
//		pTempGO->diffuseColour.g = getRandInRange<float>(0.0f, 1.0f );
//		pTempGO->diffuseColour.b = getRandInRange<float>(0.0f, 1.0f );
//		pTempGO->meshName = "bunny";
//		::g_vecGameObjects.push_back( pTempGO );
//	}

	// The object that will represent the Point light "sphere" area
	// We want this to render BOTH front and back facing triangles.
	// (Here, we have a sphere with BOTH front and back facing triangles, but we could 
	//	also render with the CullFace set to CULL_NONE, etc.)
	{
		//cGameObject* pTempGO = new cGameObject();
		::g_pLightSphere2Sided = new cGameObject();
		cPhysicalProperties physState;
		::g_pLightSphere2Sided->SetPhysState(physState);
		sMeshDrawInfo meshInfo;
		meshInfo.scale = 1.0f;
		meshInfo.debugDiffuseColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		meshInfo.name = "SmoothSphere_2_Sided_xyz_n.ply";
		meshInfo.vecMeshCubeMaps.push_back(sTextureBindBlendInfo("space", 1.0f));
		meshInfo.bIsSkyBoxObject = true;
		::g_pLightSphere2Sided->vecMeshes.push_back(meshInfo);
	}

  //// Add the debug sphere
  //{// STARTOF: Add the debug sphere
  //	::g_pTheDebugSphere = new cGameObject();
  //	::g_pTheDebugSphere->scale = 1.0f;
  //	::g_pTheDebugSphere->diffuseColour = glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f );
  //	::g_pTheDebugSphere->meshName = "SphereRadius1";
  //	::g_pTheDebugSphere->typeOfObject = eTypeOfObject::SPHERE;
  //	::g_pTheDebugSphere->radius = 1.0f;	
  //	::g_pTheDebugSphere->bIsUpdatedInPhysics = false;
  //	// NOTE: I'm NOT adding it to the vector of objects
  //	//::g_vecGameObjects.push_back( pTempGO );		// Fastest way to add
  //}// ENDOF: Add the debug sphere


	//// Add a whole bunch of spheres!
	//float limit = 200.0f;
	//float step = 25.0f;
	//for ( float x = -limit; x <= limit; x += step )
	//{
	//	for ( float y = -limit; y <= limit; y += step )
	//	{
	//		for ( float z = -limit; z <= limit; z += step )
	//		{
	//			cGameObject* pTempGO = new cGameObject();
	//			cPhysicalProperties physState;
	//			physState.position = glm::vec3(x, y, z);
	//			physState.rigidBodyShape = cPhysicalProperties::SPHERE;
	//			physState.rigidBodyMeasurements.sphere_capsule_radius = 1.0f;
	//			pTempGO->SetPhysState(physState);
	//			sMeshDrawInfo meshInfo;
	//			meshInfo.scale = step / 16.0f;
	//			meshInfo.debugDiffuseColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	//			meshInfo.name = "SphereRadius1";			// was dolphin
	//			pTempGO->vecMeshes.push_back(meshInfo);
	//			::g_vecGameObjects.push_back(pTempGO);		// Fastest way to add
	//		}
	//	}
	//}

	
	
	return;
}
