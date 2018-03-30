#include "ModelUtilities.h" 
#include "cVAOMeshManager.h"
#include "cMesh.h"
#include <sstream>
#include <vector>
#include "cGameObject.h"
#include "cModelAssetLoader.h"
#include "assimp/cAssimpBasic.h"
#include <iostream>
#include "assimp/cSimpleAssimpSkinnedMeshLoader_OneMesh.h"

// Declared in globalStuff.h
cSimpleAssimpSkinnedMesh* g_pRPGSkinnedMesh = NULL;

// Returns 0 or NULL if not found
cGameObject* findObjectByFriendlyName(std::string friendlyName, std::vector<cGameObject*> &vec_pGameObjects)
{
	// Linear search, baby!
	unsigned int numObjects = (unsigned int)vec_pGameObjects.size();
	for (unsigned int index = 0; index != numObjects; index++)
	{
		if (vec_pGameObjects[index]->friendlyName == friendlyName)
		{
			return vec_pGameObjects[index];
		}
	}
	// Didn't find it
	return NULL;
}

cGameObject* findObjectByUniqueID(unsigned int ID, std::vector<cGameObject*> &vec_pGameObjects)
{
	// Linear search, baby!
	unsigned int numObjects = (unsigned int)vec_pGameObjects.size();
	for (unsigned int index = 0; index != numObjects; index++)
	{
		if ( ID = vec_pGameObjects[index]->getUniqueID() )
		{
			return vec_pGameObjects[index];
		}
	}
	// Didn't find it
	return NULL;
}

// Called by Load3DModelsIntoMeshManager() function
bool Load3DModelHelper( std::string fileName, 
					   std::string meshFriendlyName,
					   int shaderID, 
					   cVAOMeshManager* pVAOManager,
					   cModelAssetLoader* pModelAssetLoader,
					   bool bModelHasUVs, 
					   std::string &error )
{
	bool bAllGood = true;

	cMesh tempMesh;
	tempMesh.name = meshFriendlyName;

	std::stringstream ssError;

	if (bModelHasUVs)
	{
		if (!pModelAssetLoader->LoadPlyFileIntoMeshWith_Normals_and_UV(fileName, tempMesh))
		{
			ssError << "Didn't load model >" << tempMesh.name << "<" << std::endl;
			bAllGood = false;
		}
	}
	else
	{
		if ( !pModelAssetLoader->LoadPlyFileIntoMeshWithNormals(fileName, tempMesh) )
		{
			ssError << "Didn't load model >" << tempMesh.name << "<" << std::endl;
			bAllGood = false;
		}
	}//if (bModelHasUVs)

	if ( bAllGood )
	{
			if (!pVAOManager->loadMeshIntoVAO(tempMesh, shaderID, true))
		{
			ssError << "Could not load mesh >" << tempMesh.name << "< into VAO" << std::endl;
			bAllGood = false;
		}
	}//if ( bAllGood )

	return bAllGood;
}


bool Load3DModelsIntoMeshManager( int shaderID, 
								  cVAOMeshManager* pVAOManager, 
								  cModelAssetLoader* pModelAssetLoader,
								  std::string &error )
{
	std::stringstream ssError;
	bool bAllGood = true;

	
	// *******************************************************
	// Test the assimp loader before the "old" loader
	cAssimpBasic myAssimpLoader;
	cMesh shipTestMesh;
	//if ( ! myAssimpLoader.loadModelA( "assets/modelsFBX/RPG-Character(FBX2013).FBX",
	//						           bunnyTestMesh, error ) )
	//if ( ! myAssimpLoader.loadModelA( "assets/modelsFBX/RPG-Character_Unarmed-Attack-Kick-L1(FBX2013).FBX",
	//						           bunnyTestMesh, error ) )
	//if ( ! myAssimpLoader.loadModelA( "assets/modelsFBX/ArmyPilot(FBX2013).fbx",
	//						           bunnyTestMesh, error ) )
	//if ( ! myAssimpLoader.loadModelA( "assets/models/bun_zipper_res2_xyz_n.ply",
	//						           bunnyTestMesh, error ) )
	//if ( ! myAssimpLoader.loadModelA( "assets/models/Ship_Pack_WIP_mod-command_xyz_n_uv.ply",
	//						           bunnyTestMesh, error ) )
	if (!myAssimpLoader.loadModelA("assets/models/Ship_Pack_WIP_mod-command_xyz_n_uv.obj",
		shipTestMesh, error))
	{
		std::cout << "All is lost! Forever lost!! Assimp didn't load the Ship_Pack_WIP_mod-command_xyz_n_uv.obj: "
			<< error << std::endl;
	}


	shipTestMesh.name = "Ship_Pack_WIP_mod - command_xyz_n_uv.obj";
	if (! pVAOManager->loadMeshIntoVAO(shipTestMesh, shaderID, false))
	{
		std::cout << "Assimp loaded mesh didn't load into VAO" << std::endl;
	}
	// *******************************************************



	//    ___  _    _                      _  __  __           _     
	//   / __|| |__(_) _ _   _ _   ___  __| ||  \/  | ___  ___| |_   
	//   \__ \| / /| || ' \ | ' \ / -_)/ _` || |\/| |/ -_)(_-<| ' \  
	//   |___/|_\_\|_||_||_||_||_|\___|\__,_||_|  |_|\___|/__/|_||_| 
	//                                                               
	::g_pRPGSkinnedMesh = new cSimpleAssimpSkinnedMesh();

	//	if ( ! ::g_pSkinnedMesh01->LoadMeshFromFile( "assets/modelsFBX/RPG-Character(FBX2013).FBX" ) ) 
//	if ( ! ::g_pSkinnedMesh01->LoadMeshFromFile( "assets/modelsFBX/RPG-Character_Unarmed-Attack-Kick-L1(FBX2013).FBX" ) ) 
//	if ( ! ::g_pSkinnedMesh01->LoadMeshFromFile( "assets/modelsFBX/RPG-Character_Unarmed-Walk(FBX2013).FBX" ) ) 
//	if ( ! ::g_pSkinnedMesh01->LoadMeshFromFile( "assets/modelsFBX/RPG-Character_Unarmed-Idle(FBX2013).fbx" ) ) 
//	if ( ! ::g_pSkinnedMesh01->LoadMeshFromFile( "assets/modelsMD5/hellknight/attack2.md5anim" ) ) 
//	if ( ! ::g_pSkinnedMesh01->LoadMeshFromFile( "assets/modelsFBX/RPG-Character_Unarmed-Fall(FBX2013).fbx" ) ) 
	if ( ! ::g_pRPGSkinnedMesh->LoadMeshFromFile( "assets/modelsFBX/RPG-Character(FBX2013).FBX" ) )
	{
		std::cout << "Error: problem loading the skinned mesh" << std::endl;
	}
	// Now load another animation file... 
	::g_pRPGSkinnedMesh->LoadMeshAnimation( "assets/modelsFBX/RPG-Character_Unarmed-Attack-Kick-L1(FBX2013).FBX" );
	::g_pRPGSkinnedMesh->LoadMeshAnimation( "assets/modelsFBX/RPG-Character_Unarmed-Walk(FBX2013).FBX" );
	::g_pRPGSkinnedMesh->LoadMeshAnimation( "assets/modelsFBX/RPG-Character_Unarmed-Fall(FBX2013).fbx" );
	::g_pRPGSkinnedMesh->LoadMeshAnimation( "assets/modelsFBX/RPG-Character_Unarmed-Roll-Backward(FBX2013).fbx" );
	::g_pRPGSkinnedMesh->LoadMeshAnimation( "assets/modelsFBX/RPG-Character_Unarmed-Roll-Forward(FBX2013).FBX" );
	::g_pRPGSkinnedMesh->LoadMeshAnimation("assets/modelsFBX/RPG-Character_Unarmed-Roll-Left(FBX2013).FBX" );
	::g_pRPGSkinnedMesh->LoadMeshAnimation( "assets/modelsFBX/RPG-Character_Unarmed-Roll-Right(FBX2013).FBX" );
	::g_pRPGSkinnedMesh->LoadMeshAnimation( "assets/modelsFBX/RPG-Character_Unarmed-Jump(FBX2013).FBX" );

	::g_pRPGSkinnedMesh->friendlyName = "RPG-Character";

	cMesh* pTheMesh = ::g_pRPGSkinnedMesh->CreateMeshObjectFromCurrentModel();

	if ( pTheMesh )
	{
		if ( ! pVAOManager->loadMeshIntoVAO( *pTheMesh, shaderID, false ) )
		{
			std::cout << "Could not load skinned mesh model into new VAO" << std::endl;
		}
	}
	else
	{
		std::cout << "Could not create a cMesh object from skinned mesh file" << std::endl;
	}
	// Delete temporary mesh if still around
	if ( pTheMesh )
	{
		delete pTheMesh;
	}

	//****************************************************************************************



	//{
	//	cMesh testMesh;
	//	testMesh.name = "ReallyBigShip";
	//	if ( ! pModelAssetLoader->LoadPlyFileIntoMeshWith_Normals_and_UV( "basestar_REALLY_BIG.ply", testMesh ) )
	//	{ 
	//		//std::cout << "Didn't load model" << std::endl;
	//		ssError << "Didn't load model >" << testMesh.name << "<" << std::endl;
	//		bAllGood = false;
	//	}
	//	// ***********************************************************************
	//	// NOTE the TRUE so that it keeps the mesh!!!
	//	else if ( ! pVAOManager->loadMeshIntoVAO( testMesh, shaderID, true ) )
	//	{
	//		//std::cout << "Could not load mesh into VAO" << std::endl;
	//		ssError << "Could not load mesh >" << testMesh.name << "< into VAO" << std::endl;
	//		bAllGood = false;
	//	}
	//	// ***********************************************************************

	if ( !Load3DModelHelper( "dalek2005_xyz_uv_res_2.ply", 
							 "dalek2005_xyz_uv_res_2.ply", 
							 shaderID, pVAOManager, pModelAssetLoader, true, error ) )
	{
		std::cout << error << std::endl;
	}
	
	if ( !Load3DModelHelper( "Just_Inside_Door_Frame_for_Masking.ply", 
							 "Just_Inside_Door_Frame_for_Masking.ply", 
							 shaderID, pVAOManager, pModelAssetLoader, true, error ) )
	{
		std::cout << error << std::endl;
	}
	if ( !Load3DModelHelper( "Room_2_Bigger_Triangulated.ply", 
							 "Room_2_Bigger_Triangulated.ply", 
							 shaderID, pVAOManager, pModelAssetLoader, true, error ) )
	{
		std::cout << error << std::endl;
	}

	{	// 2 sided sphere
		cMesh testMesh;
		testMesh.name = "SmoothSphere_2_Sided_xyz_n.ply";
		if ( ! pModelAssetLoader->LoadPlyFileIntoMeshWithNormals( "SmoothSphere_2_Sided_xyz_n.ply", testMesh ) )
		{ 
			//std::cout << "Didn't load model" << std::endl;
			ssError << "Didn't load model >" << testMesh.name << "<" << std::endl;
			bAllGood = false;
		}
		// ***********************************************************************
		// NOTE the TRUE so that it keeps the mesh!!!
		else if ( ! pVAOManager->loadMeshIntoVAO( testMesh, shaderID, true ) )
		{
			//std::cout << "Could not load mesh into VAO" << std::endl;
			ssError << "Could not load mesh >" << testMesh.name << "< into VAO" << std::endl;
			bAllGood = false;
		}
		// ***********************************************************************
	}	
	{
		cMesh testMesh;
		testMesh.name = "SmoothSphereRadius1";
		if ( ! pModelAssetLoader->LoadPlyFileIntoMeshWithNormals( "SmoothSphere_xyz_n.ply", testMesh ) )
		{ 
			//std::cout << "Didn't load model" << std::endl;
			ssError << "Didn't load model >" << testMesh.name << "<" << std::endl;
			bAllGood = false;
		}
		// ***********************************************************************
		// NOTE the TRUE so that it keeps the mesh!!!
		else if ( ! pVAOManager->loadMeshIntoVAO( testMesh, shaderID, true ) )
		{
			//std::cout << "Could not load mesh into VAO" << std::endl;
			ssError << "Could not load mesh >" << testMesh.name << "< into VAO" << std::endl;
			bAllGood = false;
		}
		// ***********************************************************************
	}	
	{
		cMesh testMesh;
		testMesh.name = "SmoothSphereRadius1InvertedNormals";
		if ( ! pModelAssetLoader->LoadPlyFileIntoMeshWithNormals( "SmoothSphere_Inverted_Normals_xyz_n.ply", testMesh ) )
		{ 
			//std::cout << "Didn't load model" << std::endl;
			ssError << "Didn't load model >" << testMesh.name << "<" << std::endl;
			bAllGood = false;
		}
		// ***********************************************************************
		// NOTE the TRUE so that it keeps the mesh!!!
		else if ( ! pVAOManager->loadMeshIntoVAO( testMesh, shaderID, true ) )
		{
			//std::cout << "Could not load mesh into VAO" << std::endl;
			ssError << "Could not load mesh >" << testMesh.name << "< into VAO" << std::endl;
			bAllGood = false;
		}
		// ***********************************************************************
	}	
	{
		cMesh testMesh;
		testMesh.name = "MeshLabTerrain_xyz_n_uv";
		if ( ! pModelAssetLoader->LoadPlyFileIntoMeshWith_Normals_and_UV( "MeshLabTerrain_xyz_n_uv.ply", testMesh ) )
		//if ( ! pModelAssetLoader->LoadPlyFileIntoMeshWith_Normals_and_UV( "MeshLabTerrain_FLAT_xyz_n_uv.ply", testMesh ) )
		{ 
			//std::cout << "Didn't load model" << std::endl;
			ssError << "Didn't load model >" << testMesh.name << "<" << std::endl;
			bAllGood = false;
		}
		// ***********************************************************************
		// NOTE the TRUE so that it keeps the mesh!!!
		else if ( ! pVAOManager->loadMeshIntoVAO( testMesh, shaderID, true ) )
		{
			//std::cout << "Could not load mesh into VAO" << std::endl;
			ssError << "Could not load mesh >" << testMesh.name << "< into VAO" << std::endl;
			bAllGood = false;
		}
		// ***********************************************************************
	}	
	{
		cMesh testMesh;
		testMesh.name = "teapotUV";
		if ( ! pModelAssetLoader->LoadPlyFileIntoMeshWith_Normals_and_UV( "Utah_Teapot_xyz_n_uv.ply", testMesh ) )
		{ 
			//std::cout << "Didn't load model" << std::endl;
			ssError << "Didn't load model >" << testMesh.name << "<" << std::endl;
			bAllGood = false;
		}
		else if ( ! pVAOManager->loadMeshIntoVAO( testMesh, shaderID ) )
		{
			//std::cout << "Could not load mesh into VAO" << std::endl;
			ssError << "Could not load mesh >" << testMesh.name << "< into VAO" << std::endl;
			bAllGood = false;
		}
	}

	{
		cMesh testMesh;
		testMesh.name = "bunny";
		if ( ! pModelAssetLoader->LoadPlyFileIntoMeshXYZOnly( "bun_zipper_res2_xyz.ply", testMesh ) )
		{ 
			//std::cout << "Didn't load model" << std::endl;
			ssError << "Didn't load model >" << testMesh.name << "<" << std::endl;
			bAllGood = false;
		}
		else if ( ! pVAOManager->loadMeshIntoVAO( testMesh, shaderID ) )
		{
			//std::cout << "Could not load mesh into VAO" << std::endl;
			ssError << "Could not load mesh >" << testMesh.name << "< into VAO" << std::endl;
			bAllGood = false;
		}
	}
	{
		cMesh testMesh;
		testMesh.name = "teapot";
		if ( ! pModelAssetLoader->LoadPlyFileIntoMeshXYZOnly( "Utah_Teapot_1Unit_xyz.ply", testMesh ) )
		{ 
			//std::cout << "Didn't load model" << std::endl;
			ssError << "Didn't load model >" << testMesh.name << "<" << std::endl;
			bAllGood = false;
		}
		else if ( ! pVAOManager->loadMeshIntoVAO( testMesh, shaderID ) )
		{
			//std::cout << "Could not load mesh into VAO" << std::endl;
			ssError << "Could not load mesh >" << testMesh.name << "< into VAO" << std::endl;
			bAllGood = false;
		}
	}	
	{
		cMesh testMesh;
		testMesh.name = "dolphin";
		if ( ! pModelAssetLoader->LoadPlyFileIntoMeshXYZOnly( "dolphin_xyz.ply", testMesh ) )
		{ 
			//std::cout << "Didn't load model" << std::endl;
			ssError << "Didn't load model >" << testMesh.name << "<" << std::endl;
			bAllGood = false;
		}
		else if ( ! pVAOManager->loadMeshIntoVAO( testMesh, shaderID ) )
		{
			//std::cout << "Could not load mesh into VAO" << std::endl;
			ssError << "Could not load mesh >" << testMesh.name << "< into VAO" << std::endl;
			bAllGood = false;
		}
	}	
	{
		cMesh testMesh;
		testMesh.name = "PlaneXZ";
		if ( ! pModelAssetLoader->LoadPlyFileIntoMeshXYZOnly( "Flat_XZ_Plane_xyz.ply", testMesh ) )
		{ 
			//std::cout << "Didn't load model" << std::endl;
			ssError << "Didn't load model >" << testMesh.name << "<" << std::endl;
			bAllGood = false;
		}
		else if ( ! pVAOManager->loadMeshIntoVAO( testMesh, shaderID ) )
		{
			//std::cout << "Could not load mesh into VAO" << std::endl;
			ssError << "Could not load mesh >" << testMesh.name << "< into VAO" << std::endl;
			bAllGood = false;
		}
	}	
	{
		cMesh testMesh;
		testMesh.name = "SphereRadius1";
//		if ( ! LoadPlyFileIntoMesh( "Sphereply_xyz.ply", testMesh ) )
		if ( ! pModelAssetLoader->LoadPlyFileIntoMeshWithNormals( "Sphereply_xyz_n.ply", testMesh ) )
		{ 
			//std::cout << "Didn't load model" << std::endl;
			ssError << "Didn't load model >" << testMesh.name << "<" << std::endl;
			bAllGood = false;
		}
		else if ( ! pVAOManager->loadMeshIntoVAO( testMesh, shaderID ) )
		{
			//std::cout << "Could not load mesh into VAO" << std::endl;
			ssError << "Could not load mesh >" << testMesh.name << "< into VAO" << std::endl;
			bAllGood = false;
		}
	}	// ENDOF: load models

	if ( ! bAllGood ) 
	{
		// Copy the error string stream into the error string that
		//	gets "returned" (through pass by reference)
		error = ssError.str();
	}

	return bAllGood;
}