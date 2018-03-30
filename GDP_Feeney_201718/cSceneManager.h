#ifndef _cSceneManager_HG_
#define _cSceneManager_HG_

#include <string> 

// Represents a group of asset objects being used together:
// - 3D models
// - textures and materials
// - fonts
// - animations
// - game objects (that are in the world)

// FBX and Unreal list these things in a scene or level:
// - Static Meshes
// - Skeletal Meshes
// - Animations
// - Materials (Basic support; may not match original material in your content creation app)
// - Textures
// - Rigid Mesh
// - Morph Targets
// - Cameras (no animation)
// - Lights
//

class cScene
{
public:
	cScene();
	~cScene();
	std::string name;


	
};

class cSceneManager
{
public:
	cSceneManager();
	~cSceneManager();





};



#endif
