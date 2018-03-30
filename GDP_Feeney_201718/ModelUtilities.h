#ifndef _ModelUtilities_HG_
#define _ModelUtilities_HG_

#include <string>
#include "cMesh.h"

#include "cModelAssetLoader.h"
#include "cVAOMeshManager.h"

//// Only "function signature"
//void ReadFileToToken( std::ifstream &file, std::string token );

// Note the & symbol which is "by reference" which means
//	it's NOT a copy, but referring to the original object.
// ALSO note that this is a C++ thing (& isn't a thing in C)
//bool LoadPlyFileIntoMesh( std::string filename, cMesh &theMesh );
//bool LoadPlyFileIntoMeshWithNormals( std::string filename, cMesh &theMesh );

void LoadModelsIntoScene(void);
bool Load3DModelsIntoMeshManager( int shaderID, 
								  cVAOMeshManager* pVAOManager, 
								  cModelAssetLoader* pModelAssetLoader,
								  std::string &error );


#endif
