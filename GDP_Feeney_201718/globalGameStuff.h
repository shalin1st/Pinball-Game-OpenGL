#ifndef _globalGameStuff_HG_
#define _globalGameStuff_HG_

//	 _____ _                  ___   ___  _  _ _ _____                   _ _ 
//	|_   _| |_  ___ ___ ___  |   \ / _ \| \| ( )_   _|  _ _  ___ ___ __| (_)
//	  | | | ' \/ -_|_-</ -_) | |) | (_) | .` |/  | |   | ' \/ -_) -_) _` |_ 
//	  |_| |_||_\___/__/\___| |___/ \___/|_|\_|___|_|   |_||_\___\___\__,_(_)
//	 __ _| |__ _ __| |  ___ _ _   / __| |  | __\ \    / /                   
//	/ _` | / _` / _` | / _ \ '_| | (_ | |__| _| \ \/\/ /                    
//	\__, |_\__,_\__,_| \___/_|    \___|____|_|   \_/\_/                     
//	|___/                                                                   
//
// These are files that do NOT require glad or GLFW to be included
// (So be careful what you add here)
//

#include "cGameObject.h"
#include <vector>
#include <glm/vec3.hpp>
#include "cLightManager.h"
#include "cModelAssetLoader.h"
#include "cCamera.h"
#include "Physics/cPhysicsWorld.h"

// ******************************************************************
// These require the inclusion of the OpenGL and-or GLFW headers
#include "cVAOMeshManager.h"
#include "cShaderManager.h"
//#include "Texture/cBasicTextureManager.h"
#include "Texture/CTextureManager.h"
#include "cDebugRenderer.h"
// ******************************************************************

// Remember to #include <vector>...
extern std::vector< cGameObject* >  g_vecGameObjects;

// Returns 0 or NULL if not found
cGameObject* findObjectByFriendlyName( std::string friendlyName, std::vector<cGameObject*> &vec_pGameObjects );
cGameObject* findObjectByUniqueID( unsigned int ID, std::vector<cGameObject*> &vec_pGameObjects );


extern cLightManager*	g_pLightManager;	// (theMain.cpp)

// Super basic physics update function
//void PhysicsStep( double deltaTime );
extern cPhysicsWorld*	g_pPhysicsWorld;	// (theMain.cpp)

extern cModelAssetLoader* g_pModelAssetLoader;	// (ModelUtilies.cpp)

// The teapots
//extern cGameObject* pTP0;
const std::string LEFTTEAPOTNAME = "Left Teapot";		// Now that we have a lookup

extern cCamera* g_pTheCamera;		// (theMain.cpp)


class cGameObject;	// Forward declare to avoid include

extern cVAOMeshManager*			g_pVAOManager;		// (theMain.cpp)
extern cShaderManager*			g_pShaderManager;	// (theMain.cpp)
//extern cBasicTextureManager*	g_pTextureManager;	// (theMain.cpp)
extern CTextureManager*			g_pTextureManager;	// (theMain.cpp)
extern cDebugRenderer*			g_pDebugRenderer;	// (theMain.cpp)

void RenderScene( std::vector< cGameObject* > &vec_pGOs, GLFWwindow* pGLFWWindow, double deltaTime );

extern cGameObject* g_pSkyBoxObject;	// (theMain.cpp)

extern	GLFWwindow* g_pGLFWWindow;	// In TheMain.cpp
extern bool g_IsWindowFullScreen;	// false at start
void setWindowFullScreenOrWindowed( GLFWwindow* pTheWindow, bool IsFullScreen );	// In TheMain.cpp


// Example of skinned mesh.
// NOTE: This is only ONE instance of an loaded FBX file model
extern cSimpleAssimpSkinnedMesh* g_pRPGSkinnedMesh;			// In ModelUtilites.cpp

// For stencil buffer example...
extern cGameObject* g_Room;						// in theMain.cpp
extern cGameObject* g_RoomMaskForStencil;		// in theMain.cpp


#endif
