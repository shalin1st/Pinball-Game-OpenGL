// Include glad and GLFW in correct order
#include "globalOpenGL_GLFW.h"


#include <iostream>			// C++ cin, cout, etc.
//#include "linmath.h"
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective


#include <stdlib.h>
#include <stdio.h>
// Add the file stuff library (file stream>
#include <fstream>
#include <sstream>		// "String stream"
#include <string>

#include <vector>		//  smart array, "array" in most languages
#include "Utilities.h"
#include "ModelUtilities.h"
#include "cMesh.h"
#include "sMeshDrawInfo.h"
#include "cShaderManager.h" 
#include "cGameObject.h"
#include "cVAOMeshManager.h"


//#include "Physics/Physics.h"	// Physics collision detection functions
#include "Physics/cPhysicsWorld.h"

#include "cLightManager.h"

// Include all the things that are accessed in other files
#include "globalGameStuff.h"

#include "cCamera.h"

#include "cFBO.h" 

//**********
// BE a little careful of including this everywhere...
#include "assimp/cSimpleAssimpSkinnedMeshLoader_OneMesh.h"
//**********

//#include "Dalek_Threaded_01.h"
//#include "Dalek_Threaded_02.h"
#include "Dalek_Threaded_03.h"
#include "cDalek.h"
#include"myBulletPhysics.h"
#include"myPhysics.h"
#include"myInterface.h"
iDalekManager* g_pDalekManager; 


// Here, the scene is rendered in 3 passes:
// 1. Render geometry to G buffer
// 2. Perform deferred pass, rendering to Deferred buffer
// 3. Then post-pass ("2nd pass" to the scree)
//    Copying from the Pass2_Deferred buffer to the final screen
cFBO g_FBO_Pass1_G_Buffer;
cFBO g_FBO_Pass2_Deferred;


void DrawDebugLightingSpheres(void);


cGameObject* g_pSkyBoxObject = NULL;	// (theMain.cpp)


// Remember to #include <vector>...
std::vector< cGameObject* >  g_vecGameObjects;

// Used for the light pass of the deferred rendering
cGameObject* g_pLightSphere2Sided = NULL;

cCamera* g_pTheCamera = NULL;


cVAOMeshManager* g_pVAOManager = 0;		// or NULL, or nullptr

cShaderManager*		g_pShaderManager = 0;		// Heap, new (and delete)
cLightManager*		g_pLightManager = 0;

CTextureManager*		g_pTextureManager = 0;

cModelAssetLoader*		g_pModelAssetLoader = NULL;


cDebugRenderer*			g_pDebugRenderer = 0;


// This contains the AABB grid for the terrain...
cAABBBroadPhase* g_terrainAABBBroadPhase = 0;

cPhysicsWorld*	g_pPhysicsWorld = NULL;	// (theMain.cpp)

// This is used in the "render a previous pass" object
cGameObject* g_ExampleTexturedQuad = NULL;

// For stencil buffer example...
cGameObject* g_Room = NULL;
cGameObject* g_RoomMaskForStencil = NULL;



#include "cFrameBuffer.h"

bool g_IsWindowFullScreen = false;
GLFWwindow* g_pGLFWWindow = NULL;


static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}


float g_ChromaticAberrationOffset = 0.0f;
float g_CR_Max = 0.1f;
double g_CA_CountDown = 0.0f;

const int RENDER_PASS_0_G_BUFFER_PASS = 0;
const int RENDER_PASS_1_DEFERRED_RENDER_PASS = 1;
const int RENDER_PASS_2_FULL_SCREEN_EFFECT_PASS = 2;


// Moved to GLFW_keyboardCallback.cpp
//static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)

int main(void)
{

	myInterface* yolo = new myBulletPhysics();

	//GLFWwindow* pGLFWWindow;		// Moved to allow switch from windowed to full-screen
	glfwSetErrorCallback(error_callback);



    if (!glfwInit())
	{
		// exit(EXIT_FAILURE);
		std::cout << "ERROR: Couldn't init GLFW, so we're pretty much stuck; do you have OpenGL??" << std::endl;
		return -1;
	}

	int height = 480;	/* default */
	int width = 640;	// default
	std::string title = "OpenGL Rocks";

	std::ifstream infoFile("config.txt");
	if ( ! infoFile.is_open() )
	{	// File didn't open...
		std::cout << "Can't find config file" << std::endl;
		std::cout << "Using defaults" << std::endl;
	}   
	else
	{	// File DID open, so read it... 
		std::string a;	
		
		infoFile >> a;	// "Game"	//std::cin >> a;
		infoFile >> a;	// "Config"
		infoFile >> a;	// "width"

		infoFile >> width;	// 1080

		infoFile >> a;	// "height"

		infoFile >> height;	// 768

		infoFile >> a;		// Title_Start

		std::stringstream ssTitle;		// Inside "sstream"
		bool bKeepReading = true;
		do
		{
			infoFile >> a;		// Title_End??
			if ( a != "Title_End" )
			{
				ssTitle << a << " ";
			}
			else
			{	// it IS the end! 
				bKeepReading  = false;
				title = ssTitle.str();
			}
		}while( bKeepReading );


	}//if ( ! infoFile.is_open() )




    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	// C++ string
	// C no strings. Sorry. char    char name[7] = "Michael\0";
    ::g_pGLFWWindow = glfwCreateWindow( width, height, 
							            title.c_str(), 
							            NULL, NULL);
    if ( ! ::g_pGLFWWindow )
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback( ::g_pGLFWWindow, key_callback );
	// For the FBO to resize when the window changes
	glfwSetWindowSizeCallback( ::g_pGLFWWindow, window_size_callback );

    glfwMakeContextCurrent( ::g_pGLFWWindow );
    gladLoadGLLoader( (GLADloadproc) glfwGetProcAddress );
    glfwSwapInterval(1);



	std::cout << glGetString(GL_VENDOR) << " " 
		<< glGetString(GL_RENDERER) << ", " 
		<< glGetString(GL_VERSION) << std::endl;
	std::cout << "Shader language version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	// General error string, used for a number of items during start up
	std::string error;

	::g_pShaderManager = new cShaderManager();

	cShaderManager::cShader vertShader;
	cShaderManager::cShader fragShader;

	vertShader.fileName = "simpleVert.glsl";	
	//fragShader.fileName = "simpleFrag.glsl"; 
	fragShader.fileName = "simpleFragDeferred.glsl"; 

	::g_pShaderManager->setBasePath( "assets//shaders//" );

	// Shader objects are passed by reference so that
	//	we can look at the results if we wanted to. 
	if ( ! ::g_pShaderManager->createProgramFromFile(
		        "mySexyShader", vertShader, fragShader ) )
	{
		std::cout << "Oh no! All is lost!!! Blame Loki!!!" << std::endl;
		std::cout << ::g_pShaderManager->getLastError() << std::endl;
		// Should we exit?? 
		return -1;	
//		exit(
	}
	std::cout << "The shaders comipled and linked OK" << std::endl;


// Triangle debug renderer test...
	::g_pDebugRenderer = new cDebugRenderer();
	if ( ! ::g_pDebugRenderer->initialize(error) )
	{
		std::cout << "Warning: couldn't init the debug renderer." << std::endl;
		std::cout << "\t" << ::g_pDebugRenderer->getLastError() << std::endl;
	}

//	const float WORLDMAX = 25.0f;
//	::g_pDebugRenderer->addTriangle( glm::vec3( -WORLDMAX, 0.0f, 0.0f ),
//									 glm::vec3(WORLDMAX, 0.0f, 0.0f ),
//									 glm::vec3( 0.0f, WORLDMAX, 0.0f),
//									 glm::vec3( 1.0f, 1.0f, 1.0f ), 20.0f );
//	for (int count = 0; count != 100; count++)
//	{
//		::g_pDebugRenderer->addTriangle(
//			glm::vec3(getRandInRange(-WORLDMAX, WORLDMAX),
//			          getRandInRange(-WORLDMAX, WORLDMAX),
//			          getRandInRange(-WORLDMAX, WORLDMAX)),
//			glm::vec3(getRandInRange(-WORLDMAX, WORLDMAX), 
//			          getRandInRange(-WORLDMAX, WORLDMAX),
//			          getRandInRange(-WORLDMAX, WORLDMAX)),
//			glm::vec3(getRandInRange(-WORLDMAX, WORLDMAX),
//			          getRandInRange(-WORLDMAX, WORLDMAX),
//			          getRandInRange(-WORLDMAX, WORLDMAX)),
//			glm::vec3( 1.0f, 1.0f, 1.0f ), 15.0f );
//	}//for ...
//	::g_pDebugRenderer->addTriangle( glm::vec3( -50.0f, -25.0f, 0.0f ),
//									 glm::vec3( 0.0, 50.0f, 100.0f ),
//									 glm::vec3( 50.0f, -25.0, 0.0f),
//									 glm::vec3( 1.0f, 1.0f, 0.0f ), 1000.0f );


//***********************************************************
//***********************************************************
	const int NUMBER_OF_DALEKS = 25;

//	::g_pDalekManager = new cDalekManager01();
//	::g_pDalekManager = new cDalekManager02();
	::g_pDalekManager = new cDalekManager03();
	::g_pDalekManager->Init(NUMBER_OF_DALEKS);

	// Local array to point the Daleks
	std::vector<glm::vec3> vecDalekPositions;
	vecDalekPositions.reserve(NUMBER_OF_DALEKS);
	for ( int count = 0; count != NUMBER_OF_DALEKS; count++ )
	{
		vecDalekPositions.push_back(glm::vec3(0.0f));
	}

	std::map< unsigned int /*Dalek Index*/, cGameObject* /*Dalek Game Object*/> mapDalekID_to_GameObject;

	const float WORLDLIMIT = 1000.0f;
	for ( int count = 0; count != NUMBER_OF_DALEKS; count++ )
	{
		glm::vec3 position;
		position.x = getRandInRange<float>( -WORLDLIMIT, WORLDLIMIT );
		position.y = 0.0f;
		position.z = getRandInRange<float>( -WORLDLIMIT, WORLDLIMIT );
		cGameObject* pCurDalekGO = MakeDalekGameObject(position);

		cDalek* pCurrentDalek = NULL;
		::g_pDalekManager->CreateDalekThread( pCurDalekGO, pCurrentDalek );

		mapDalekID_to_GameObject[pCurrentDalek->getDalekID()] = pCurDalekGO;

		::g_vecGameObjects.push_back( pCurDalekGO );
	}
//***********************************************************
//***********************************************************



	// Load models
	::g_pModelAssetLoader = new cModelAssetLoader();
	::g_pModelAssetLoader->setBasePath("assets/models/");

	::g_pVAOManager = new cVAOMeshManager();

	GLint currentProgID = ::g_pShaderManager->getIDFromFriendlyName("mySexyShader");


	GLint sexyShaderID = ::g_pShaderManager->getIDFromFriendlyName("mySexyShader");

	if ( ! Load3DModelsIntoMeshManager(sexyShaderID, ::g_pVAOManager, ::g_pModelAssetLoader, error ) )
	{
		std::cout << "Not all models were loaded..." << std::endl;
		std::cout << error << std::endl;
	}

	LoadModelsIntoScene();



	// Named unifrom block
	// Now many uniform blocks are there? 
	GLint numberOfUniformBlocks = -1;
	glGetProgramiv(currentProgID, GL_ACTIVE_UNIFORM_BLOCKS, &numberOfUniformBlocks);

	// https://www.opengl.org/wiki/GLAPI/glGetActiveUniformBlock

	// Set aside some buffers for the names of the blocks
	const int BUFFERSIZE = 1000;

	int name_length_written = 0;

	char NUBName_0[BUFFERSIZE] = {0};
	char NUBName_1[BUFFERSIZE] = {0};

	glGetActiveUniformBlockName(currentProgID,
								0,
								BUFFERSIZE,
								&name_length_written,
								NUBName_0);

	glGetActiveUniformBlockName(currentProgID,
								1,
								BUFFERSIZE,
								&name_length_written,
								NUBName_1);

//	NUB_lighting
//	NUB_perFrame

	GLuint NUB_Buffer_0_ID = 0;
	GLuint NUB_Buffer_1_ID = 0;

	glGenBuffers(1, &NUB_Buffer_0_ID);
	glBindBuffer(GL_UNIFORM_BUFFER, NUB_Buffer_0_ID );

	glGenBuffers(1, &NUB_Buffer_1_ID);
	glBindBuffer(GL_UNIFORM_BUFFER, NUB_Buffer_1_ID);




	// Get the uniform locations for this shader
//	mvp_location = glGetUniformLocation(currentProgID, "MVP");		// program, "MVP");


//	GLint uniLoc_diffuseColour = glGetUniformLocation( currentProgID, "diffuseColour" );

	::g_pLightManager = new cLightManager();

	::g_pLightManager->CreateLights(10);	// There are 10 lights in the shader

	::g_pLightManager->vecLights[0].setLightParamType(cLight::POINT);		
	::g_pLightManager->vecLights[0].position = glm::vec3(500.0f, 500.0f, 1000.0f);
	::g_pLightManager->vecLights[0].attenuation.y = 0.0002f;		


	::g_pLightManager->vecLights[1].position = glm::vec3(0.0f, 570.0f, -212.0f);	
	::g_pLightManager->vecLights[1].attenuation.y = 0.000456922280f;		//0.172113f;
	::g_pLightManager->vecLights[1].setLightParamType(cLight::SPOT);		// <--- DOH! This would explain why I couldn't light up the scene!!
	// Point spot straight down at the ground
	::g_pLightManager->vecLights[1].direction = glm::vec3(0.0f, -1.0f, 0.0f );
	::g_pLightManager->vecLights[1].setLightParamSpotPrenumAngleInner( glm::radians(15.0f) );
	::g_pLightManager->vecLights[1].setLightParamSpotPrenumAngleOuter( glm::radians(45.0f) );
	::g_pLightManager->vecLights[1].position = glm::vec3(0.0f, 50.0f, 0.0f);	


	::g_pLightManager->LoadShaderUniformLocations(currentProgID);



	// Texture 
	::g_pTextureManager = new CTextureManager();

	std::cout << "GL_MAX_TEXTURE_IMAGE_UNITS: " << ::g_pTextureManager->getOpenGL_GL_MAX_TEXTURE_IMAGE_UNITS() << std::endl;
	std::cout << "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS: " << ::g_pTextureManager->getOpenGL_GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS() << std::endl;

	::g_pTextureManager->setBasePath("assets/textures");
	if ( ! ::g_pTextureManager->Create2DTextureFromBMPFile("Utah_Teapot_xyz_n_uv_Checkerboard.bmp", true) )
	{
		std::cout << "Didn't load the texture. Oh no!" << std::endl;
	}
	else
	{
		std::cout << "Texture is loaded! Hazzah!" << std::endl;
	}
	::g_pTextureManager->Create2DTextureFromBMPFile("Utah_Teapot_xyz_n_uv_Enterprise.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("GuysOnSharkUnicorn.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("GuysOnSharkUnicorn.bmp", true);
//	::g_pTextureManager->Create2DTextureFromBMPFile("Seamless_ground_sand_texture.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("barberton_etm_2001121_lrg.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("height_map_norway-height-map-aster-30m.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("A_height_map_norway-height-map-aster-30m.bmp", true);

	::g_pTextureManager->setBasePath("assets/textures/skybox");
	if (!::g_pTextureManager->CreateCubeTextureFromBMPFiles(
			"space",
			"SpaceBox_right1_posX.bmp",
			"SpaceBox_left2_negX.bmp",
			"SpaceBox_top3_posY.bmp",
			"SpaceBox_bottom4_negY.bmp",
			"SpaceBox_front5_posZ.bmp",
			"SpaceBox_back6_negZ.bmp", true, true))
	{
		std::cout << "Didn't load skybox" << std::endl;
	}


	      
///***********************************************************
	// About the generate the AABB for the terrain
	::g_terrainAABBBroadPhase = new cAABBBroadPhase();
	// Perhaps you'd like something more sophisticated than this...
	::g_terrainAABBBroadPhase->pDebugRenderer = ::g_pDebugRenderer;
	// 
	cMesh terrainMesh; 
	if (::g_pVAOManager->lookupMeshFromName("MeshLabTerrain_xyz_n_uv", terrainMesh))
	{
		std::cout << "Generating the terrain AABB grid. This will take a moment..." << std::endl;
		
		::g_terrainAABBBroadPhase->genAABBGridFromMesh(terrainMesh);

	}//if (::g_pVAOManager->lookupMeshFromName
///***********************************************************


	::g_pTheCamera = new cCamera();
//	::g_pTheCamera->setCameraMode(cCamera::FLY_CAMERA);
//	::g_pTheCamera->eye = glm::vec3(0.0f, 5.0f, -10.0f);

	//::g_pTheCamera->setCameraMode(cCamera::FOLLOW_CAMERA);
	//::g_pTheCamera->eye = glm::vec3(-100.0f, 150.0f, -300.0f);
	//::g_pTheCamera->Follow_SetMaxFollowSpeed(3.0f);
	//::g_pTheCamera->Follow_SetDistanceMaxSpeed(50.0f);	// Full speed beyond this distance
	//::g_pTheCamera->Follow_SetDistanceMinSpeed(25.0f);	// Zero speed at this distance
	//::g_pTheCamera->Follow_SetOrUpdateTarget(glm::vec3(0.0f, 0.0f, 0.0f));
	//::g_pTheCamera->Follow_SetIdealCameraLocation(glm::vec3(0.0f, 5.0f, 5.0f));

	::g_pTheCamera->setCameraMode(cCamera::MODE_FLY_USING_LOOK_AT);
	::g_pTheCamera->FlyCamLA->setEyePosition(glm::vec3(0.0f, 10.0f, 650.0f));
	::g_pTheCamera->FlyCamLA->setTargetInWorld(glm::vec3(0.0f, 20.0f, 0.0f));
	::g_pTheCamera->FlyCamLA->setUpVector(glm::vec3(0.0f, 1.0f, 0.0f));

//	::g_pTheCamera->FollowCam->SetOrUpdateTarget(glm::vec3(1.0f));


	// Follow the teapot
	cGameObject* pLeftTeapot = findObjectByFriendlyName(LEFTTEAPOTNAME, ::g_vecGameObjects);
	cPhysicalProperties physState;
	pLeftTeapot->GetPhysState(physState);
	physState.position.x = 150.0f;
	physState.velocity.x = -10.0f;
	physState.velocity.y = +25.0f;
	physState.integrationUpdateType = cPhysicalProperties::DYNAMIC;	//	pLeftTeapot->bIsUpdatedInPhysics = true;
	//pLeftTeapot->SetPhysState(physState);
	//::g_pTheCamera->Follow_SetOrUpdateTarget(physState.position);


	::g_pPhysicsWorld = new cPhysicsWorld();


	// All loaded!
	std::cout << "And we're good to go! Staring the main loop..." << std::endl;

	glEnable( GL_DEPTH );


	// Create an FBO
	if ( ! ::g_FBO_Pass1_G_Buffer.init(1920, 1080, error) )
	{
		std::cout << "::g_FBO_Pass2_Deferred error: " << error << std::endl;
	}
	else
	{
		std::cout << "::g_FBO_Pass2_Deferred is good." << std::endl;
		std::cout << "\t::g_FBO_Pass1_G_Buffer ID = " << ::g_FBO_Pass1_G_Buffer.ID << std::endl;
		std::cout << "\tcolour texture ID = " << ::g_FBO_Pass1_G_Buffer.colourTexture_0_ID << std::endl;
		std::cout << "\tnormal texture ID = " << ::g_FBO_Pass1_G_Buffer.normalTexture_1_ID << std::endl;

		std::cout << "GL_MAX_COLOR_ATTACHMENTS = " << ::g_FBO_Pass1_G_Buffer.getMaxColourAttachments() << std::endl;
		std::cout << "GL_MAX_DRAW_BUFFERS = " << ::g_FBO_Pass1_G_Buffer.getMaxDrawBuffers() << std::endl;

	}//if ( ! ::g_FBO_Pass1_G_Buffer.init

	if ( ! ::g_FBO_Pass2_Deferred.init(1920, 1080, error) )
	{
		std::cout << "::g_FBO_Pass2_Deferred error: " << error << std::endl;
	}
	else
	{
		std::cout << "FBO is good." << std::endl;
		std::cout << "\t::g_FBO_Pass2_Deferred ID = " << ::g_FBO_Pass2_Deferred.ID << std::endl;
		std::cout << "\tcolour texture ID = " << ::g_FBO_Pass2_Deferred.colourTexture_0_ID << std::endl;
		std::cout << "\tnormal texture ID = " << ::g_FBO_Pass2_Deferred.normalTexture_1_ID << std::endl;

		std::cout << "GL_MAX_COLOR_ATTACHMENTS = " << ::g_FBO_Pass2_Deferred.getMaxColourAttachments() << std::endl;
		std::cout << "GL_MAX_DRAW_BUFFERS = " << ::g_FBO_Pass2_Deferred.getMaxDrawBuffers() << std::endl;

	}//if ( ! ::g_FBO_Pass2_Deferred.init




	setWindowFullScreenOrWindowed( ::g_pGLFWWindow, ::g_IsWindowFullScreen );


	// Gets the "current" time "tick" or "step"
	double lastTimeStep = glfwGetTime();






	//// Spawn the tread to update the Dalek 
	//unsigned int DalekThreadHandle = 0;
	//unsigned threadID = 0;

	//// Creates critical section, etc.
	//InitDalekThreading();

	//DalekThreadHandle = _beginthreadex(
	//	NULL,			// Don't change the security permissions for this thread
	//	0,				// 0 = default stack
	//	DalekBrainThread,		// The function we want to call. 
	//	NULL, //pDataToPassA,			// Arguement list (or NULL if we are passing VOID)
	//	0,				// or CREATE_SUSPENDED if it's paused and has to start
	//	&threadID);


	// Allow the Daleks to update
	::g_pDalekManager->SetIsUpdatingOnAllDaleks(true);

	// Could also:
	// 1. Start the threads as CREATE_SUSPENDED
	// 2. Then go through all the threads and call 
	//    ResumeThread() on each one. 
	// Never call SuspendThead


	bool bHACK_RUN_ONCE_TO_SET_LIGHT = false;

	// Main game or application loop
	while ( ! glfwWindowShouldClose(::g_pGLFWWindow) )
    {
		// Essentially the "frame time"
		// Now many seconds that have elapsed since we last checked
		double curTime = glfwGetTime();
		double deltaTime =  curTime - lastTimeStep;
		lastTimeStep = curTime;

		// Call the "thread" function
		//::g_DeltaTime = deltaTime;
//		::g_SetDeltaTime(deltaTime);
//		DalekBrainThread(NULL);
//
//		// Copy the Dalek information to the actual game object
//		cGameObject* pDalekGO = findObjectByFriendlyName("Big D", ::g_vecGameObjects);
//		cPhysProps state;
//		pDalekGO->GetPhysState(state);
////		state.position = ::g_DalekPosition;
//		state.position = ::g_GetDalekPosition();
//		pDalekGO->SetPhysState( state );
//		std::cout 
//			<< state.position.x << ", " 
//			<< state.position.y << ", " 
//			<< state.position.z << std::endl;
//
		// ************************************************************
//		mapDalekID_to_GameObject;
//			virtual bool getDalekPositionsAtIndexRange( unsigned int startIndex, unsigned int endIndex, 
//									             );
		::g_pDalekManager->getAllDalekPositions( vecDalekPositions );
		for ( unsigned int index = 0; index != vecDalekPositions.size(); index++ )
		{
			cPhysProps DalekPhysState;
			cGameObject* pDalekGO = mapDalekID_to_GameObject[index];
			pDalekGO->GetPhysState( DalekPhysState );
			DalekPhysState.position = vecDalekPositions[index];
			pDalekGO->SetPhysState( DalekPhysState );
		}

		// For lockless, swich buffers
		::g_pDalekManager->SwitchBuffers();

		// ************************************************************



		::g_pPhysicsWorld->IntegrationStep(deltaTime);

		::g_pTheCamera->updateTick(deltaTime);

		// *********************************************
		//    ___ _        ___              __     ___                          
		//   / __| |___  _| _ ) _____ __  __\ \   / __|__ _ _ __  ___ _ _ __ _  
		//   \__ \ / / || | _ \/ _ \ \ / |___> > | (__/ _` | '  \/ -_) '_/ _` | 
		//   |___/_\_\\_, |___/\___/_\_\    /_/   \___\__,_|_|_|_\___|_| \__,_| 
		//            |__/                                                      
		cPhysicalProperties skyBoxPP;
		::g_pSkyBoxObject->GetPhysState(skyBoxPP);
		skyBoxPP.position = ::g_pTheCamera->getEyePosition();
		::g_pSkyBoxObject->SetPhysState(skyBoxPP);



//    ___                _             _           ___       _            __   __           
//   | _ \ ___  _ _   __| | ___  _ _  | |_  ___   / __| ___ | |__  _  _  / _| / _| ___  _ _ 
//   |   // -_)| ' \ / _` |/ -_)| '_| |  _|/ _ \ | (_ ||___|| '_ \| || ||  _||  _|/ -_)| '_|
//   |_|_\\___||_||_|\__,_|\___||_|    \__|\___/  \___|     |_.__/ \_,_||_|  |_|  \___||_|  
//                                                                        
// In this pass, we render all the geometry to the "G buffer"
// The lighting is NOT done here. 
// 
		::g_pShaderManager->useShaderProgram("mySexyShader");

		// Direct everything to the FBO
		GLint renderPassNumber_LocID = glGetUniformLocation(sexyShaderID, "renderPassNumber");
		glUniform1i( renderPassNumber_LocID, RENDER_PASS_0_G_BUFFER_PASS );

		glBindFramebuffer(GL_FRAMEBUFFER, g_FBO_Pass1_G_Buffer.ID );
		// Clear colour AND depth buffer
		g_FBO_Pass1_G_Buffer.clearBuffers();

//glDisable(GL_DEPTH_TEST);		// Check what's already in the depth buffer
//glDepthMask(GL_FALSE);			// Written to the depth buffer

		// 1. Drawing the "mask" object (that "stencil")

		// Note the addition to the stencil buffer clear
		// BIG NOTE: This clears the FINAL frame buffer! 
		// (Keep in mind that when the stencil is enabled, it's enabled for ALL frame buffers)
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

		glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );	// Clear colour is BLACK

//////////		glEnable(GL_STENCIL_TEST);
//////////
//////////		// Always SUCCEED (the stencil test).
//////////		// Don't mask anything (0xFF means don't "mask")
//////////		// Write "ref" to buffer if succeeds.
//////////		// NOTE: 
//////////		// - Stencil is currently all zeros. 
//////////		// - Whatever we draw will be written to the stencil as a value of 1
//////////		// - The mask of 0xFF (1111 1111) means nothing will be masked (prevented)
////////////		glStencilFunc(GL_ALWAYS, 1, 0xFF);		// All 1s: 1111 1111
//////////		glStencilFunc(GL_GEQUAL, 1, 0xFF);		// All 1s: 1111 1111
//////////		// Stencil will ALWAYS pass...
//////////		// Depth will pass, too (since this is the only thing we are drawing)
//////////		// If the Stencil AND the Depth PASS, then REPLACE the stencil value with the 
//////////		// ...value in the StencilFun(), which is the value 1, in this case
//////////		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
//////////		glStencilMask(0xFF);		// Control of writing to the buffer
//////////		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
//////////		// Render the doorway mask
////////////	glDisable(GL_DEPTH_TEST);
////////////	glDepthMask(GL_FALSE);
//////////
//////////		std::vector< cGameObject* > vecOnlyTheRoomMask;
//////////		vecOnlyTheRoomMask.push_back( ::g_RoomMaskForStencil);
//////////		RenderScene(vecOnlyTheRoomMask, ::g_pGLFWWindow, deltaTime );
//////////		// So, at this point the stencil buffer will be zero (0), except where we 
//////////		// ...drew the door mask object, where it will be one (1)
//////////
//////////
//////////		// 2. 
//////////		// Clear colour buffer ONLY
//////////		// Keep depth and stencil: 
//////////		//  - depth because we don't want to draw what's behind the mask
//////////		//  - stencil is untouched
//////////		//  - clear colour to "erase" the masking object.
////////////		glClear(GL_COLOR_BUFFER_BIT );
////////////		glClear(GL_DEPTH_BUFFER_BIT );
//////////
//////////		// Draw only the room
//////////		// Where it's 0, draw the room (where we DIDN'T draw the masking object)
//////////		// (Note the bit mask is 0xFF, meaning we aren't selecting a particular stencil)
//////////		glStencilFunc(GL_ALWAYS, 0, 0xFF);
//////////		// Stencil passes (always) - keep original
//////////		// AND Depth FAILS - Keep stencil (this is where the mask is IN FRONT OF portions of the room, like the far side)
//////////		// Stencil AND depth pass - REPLACE with a value of zero (meaning part of the room is IN FRONT OF the door mask)
//////////		// Parts of the room are in front of the door mask, but since the door mask was drawn 1st, the stencil
//////////		//	buffer will have 1s at those locations. These "in front of the mask" will now be set back to zeros.
//////////		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
//////////		// Draw ONLY the room
//////////		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
////////////	glEnable(GL_DEPTH_TEST);
////////////	glDepthMask(GL_TRUE);
//////////		std::vector< cGameObject* > vecOnlyTheRoom;
//////////		vecOnlyTheRoom.push_back( ::g_Room );
//////////		RenderScene( vecOnlyTheRoom, ::g_pGLFWWindow, deltaTime );


		// 3. Draw the rest of the scene.
		// Clear the depth buffer, too (where the door mask was)
		glClear( GL_DEPTH_BUFFER_BIT );
//		glDisable(GL_STENCIL_TEST);
		// Where it's 1, draw the scene 
		// - remember, wherever we drew the masking object, it's 1
		// - the rest of the scene is sill zero 
		// So this passes where every the 0 is less than the stencil buffer
		// - The stencil has 1 where the mask is, and zero everywhere else
		glStencilFunc(GL_EQUAL, 1, 0xFF);
		glStencilMask(0x00);		// Make sure we can't write to the stencil buffer
		// BUT, we DON'T want to change what's already on the stencil buffer
		glStencilOp(GL_KEEP,		// Stencil fail
					GL_KEEP,		// Depth fail
					GL_KEEP);		// Stencil AND Depth PASS

		RenderScene( ::g_vecGameObjects, ::g_pGLFWWindow, deltaTime );

		// At this pointer EVERYTHING in the scene is rendered to the "G-Buffer"


//// *********************************************************************************
//		// NOTE: The RenderScene eventually updates the skinned mesh info, 
//		//			which is now stored into the skinned mesh information
//		// When I call this, if it's a skinned mesh, the extents will be calculated, too
//		// (like for that pose on this frame)
//		// Find one of the skinned meshes
//		cGameObject* pSophieT = findObjectByFriendlyName( "Sophie", ::g_vecGameObjects );
//
//		// This box is "object relative" (i.e. around origin of the model, not in the world)
//		glm::vec3 minXYZ = pSophieT->vecMeshes[0].minXYZ_from_SM_Bones;
//		glm::vec3 maxXYZ = pSophieT->vecMeshes[0].maxXYZ_from_SM_Bones;
//
////	// Transform this based on where the character is in the world...
//	// BUT HOW, you? If only there was a matrix that described where the object was!!!
//	{
//		cPhysicalProperties sophPhysState;
//		pSophieT->GetPhysState(sophPhysState);
//
//		glm::mat4 matSophieWorld = glm::mat4(1.0f);		// identity
//		glm::mat4 trans = glm::mat4x4(1.0f);
//		matSophieWorld = glm::translate(matSophieWorld, sophPhysState.position );
//		glm::mat4 postRotQuat = glm::mat4(pSophieT->getFinalMeshQOrientation(sophPhysState.get));
//		matSophieWorld = matSophieWorld * postRotQuat;
//		glm::mat4 matScale = glm::mat4x4(1.0f);
//		matScale = glm::scale(matScale,
//							  glm::vec3(pSophieT->GetPhysState,
//							  theMesh.scale,
//							  theMesh.scale));
//		matSophieWorld = matSophieWorld * matScale;
//	}
//
//		//pSophieT->vecMeshes[0].vecObjectBoneTransformation[boneIndex]
//
//		g_pDebugRenderer->addTriangle( minXYZ, maxXYZ, minXYZ,
//									   glm::vec3(0.0,0.0,0.0) );
//		g_pDebugRenderer->addTriangle( maxXYZ, minXYZ,maxXYZ, 
//									   glm::vec3(0.0,0.0,0.0) );
//
//		// Draw a triangle at each bone... 
//		// Note we have to translate the location for each debug triangle.
//		// (but the "boneLocationXYZ" is the centre of the bone)
////		for (unsigned int boneIndex = 0; 
////			 boneIndex != pSophieT->vecMeshes[0].vecObjectBoneTransformation.size();
////			 boneIndex++)
////		{
////			glm::mat4 boneLocal = pSophieT->vecMeshes[0].vecObjectBoneTransformation[boneIndex];
////	
////			float scale = pSophieT->vecMeshes[0].scale;
////			boneLocal = glm::scale(boneLocal, glm::vec3(scale, scale, scale));
////	
////	
////			//glm::vec4 boneBallLocation = boneLocal * GameObjectLocalOriginLocation;
////			glm::vec4 boneLocationXYZ = boneLocal * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
////			boneLocationXYZ *= scale;
////	
////			// Draw a triangle at each bone...
////			glm::vec3 v[3];
////			v[0].x = boneLocationXYZ.x;
////			v[0].y = boneLocationXYZ.y;
////			v[0].z = boneLocationXYZ.z;
////			v[1] = v[0] + glm::vec3(2.0f, 0.0f, 0.0f);
////			v[2] = v[0] - glm::vec3(2.0f, 0.0f, 0.0f);
////	
////			g_pDebugRenderer->addTriangle( v[0], v[1], v[2], glm::vec3(1.0f, 1.0f, 1.0f) ); 
////		}
//
//		int indexOfFingerBone = pSophieT->pSimpleSkinnedMesh->m_mapBoneNameToBoneIndex["B_L_Finger02"];// = 32
//
//		// Transform to get the location in world space... 
//
//		// This vector of bones has the final location of that bone (this frame)
//		glm::mat4 boneLocal = pSophieT->vecMeshes[0].vecObjectBoneTransformation[indexOfFingerBone];
//
//		float scale = pSophieT->vecMeshes[0].scale;
//		boneLocal = glm::scale(boneLocal, glm::vec3(scale, scale, scale));
//
//		glm::vec4 B_L_Finger02_XYZ = boneLocal * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
//		B_L_Finger02_XYZ *= scale;
//		// And transform based on object world...
//
//		// matWorld (or matObject) of the object
//
//		glm::vec3 fingerXYZ = glm::vec3(B_L_Finger02_XYZ);
//
//		g_pDebugRenderer->addTriangle( fingerXYZ,
//									   fingerXYZ + glm::vec3(1.0,0,0),
//									   fingerXYZ - glm::vec3(1.0,0,0),
//									   glm::vec3(1.0f, 1.0f, 1.0f) );
//// *********************************************************************************

//    ___         __                         _   ___                _             ___               
//   |   \  ___  / _| ___  _ _  _ _  ___  __| | | _ \ ___  _ _   __| | ___  _ _  | _ \ __ _  ___ ___
//   | |) |/ -_)|  _|/ -_)| '_|| '_|/ -_)/ _` | |   // -_)| ' \ / _` |/ -_)| '_| |  _// _` |(_-<(_-<
//   |___/ \___||_|  \___||_|  |_|  \___|\__,_| |_|_\\___||_||_|\__,_|\___||_|   |_|  \__,_|/__//__/
//   
// In this pass, we READ from the G-buffer, and calculate the lighting. 
// In this example, we are writing to another FBO, now. 
// 


		// Render it again, but point the the FBO texture... 
		glBindFramebuffer(GL_FRAMEBUFFER, g_FBO_Pass2_Deferred.ID );
		//g_FBO_Pass2_Deferred.clearBuffers();
// ************************************
		// SETS THE CLEAR SCREEN COLOUR TO RED (to show outline of light volume)
		g_FBO_Pass2_Deferred.clearBuffers(glm::vec4(1.0f, 0.0, 0.0, 1.0f));
// ************************************


		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
		glDisable(GL_STENCIL_TEST);

		::g_pShaderManager->useShaderProgram("mySexyShader");

		glUniform1i( renderPassNumber_LocID, RENDER_PASS_1_DEFERRED_RENDER_PASS );
		
		//uniform sampler2D texFBONormal2D;
		//uniform sampler2D texFBOVertexWorldPos2D;

		GLint texFBOColour2DTextureUnitID = 10;
		GLint texFBOColour2DLocID = glGetUniformLocation(sexyShaderID, "texFBOColour2D");
		GLint texFBONormal2DTextureUnitID = 11;
		GLint texFBONormal2DLocID = glGetUniformLocation(sexyShaderID, "texFBONormal2D");
		GLint texFBOWorldPosition2DTextureUnitID = 12;
		GLint texFBOWorldPosition2DLocID = glGetUniformLocation(sexyShaderID, "texFBOVertexWorldPos2D");

		// Pick a texture unit... 
		glActiveTexture(GL_TEXTURE0 + texFBOColour2DTextureUnitID);
		glBindTexture(GL_TEXTURE_2D, g_FBO_Pass1_G_Buffer.colourTexture_0_ID);
		glUniform1i(texFBOColour2DLocID, texFBOColour2DTextureUnitID);

		glActiveTexture(GL_TEXTURE0 + texFBONormal2DTextureUnitID);
		glBindTexture(GL_TEXTURE_2D, g_FBO_Pass1_G_Buffer.normalTexture_1_ID);
		glUniform1i(texFBONormal2DLocID, texFBONormal2DTextureUnitID);
		
		glActiveTexture(GL_TEXTURE0 + texFBOWorldPosition2DTextureUnitID);
		glBindTexture(GL_TEXTURE_2D, g_FBO_Pass1_G_Buffer.vertexWorldPos_2_ID);
		glUniform1i(texFBOWorldPosition2DLocID, texFBOWorldPosition2DTextureUnitID);
		
		// Set the sampler in the shader to the same texture unit (20)

		glfwGetFramebufferSize(::g_pGLFWWindow, &width, &height);

		GLint screenWidthLocID = glGetUniformLocation(sexyShaderID, "screenWidth");
		GLint screenHeightLocID = glGetUniformLocation(sexyShaderID, "screenHeight");
		glUniform1f(screenWidthLocID, (float)width);
		glUniform1f(screenHeightLocID, (float)height);

		// Adjust the CA:
		//float g_ChromaticAberrationOffset = 0.0f;
		// 0.1
		if ( ::g_CA_CountDown > 0.0 )
		{
			float CRChange = getRandInRange( (-g_CR_Max/10.0f), (::g_CR_Max/10.0f) );
			::g_ChromaticAberrationOffset += CRChange;
			::g_CA_CountDown -= deltaTime;
		}
		else
		{
			::g_ChromaticAberrationOffset = 0.0f;
		}

		GLint CROffset_LocID = glGetUniformLocation(sexyShaderID, "CAoffset" );
		glUniform1f( CROffset_LocID, g_ChromaticAberrationOffset);

		std::vector< cGameObject* >  vecLightAccumulationPass;
		// Push back a SINGLE quad or GIANT triangle that fills the entire screen
		// Here we will use the skybox (as it fills the entire screen)
		//vecLightAccumulationPass.push_back( ::g_pSkyBoxObject );
//		cGameObject* pBunny = findObjectByFriendlyName("bugs", ::g_vecGameObjects);
//		vecCopy2ndPass.push_back(pBunny);
		

		// Use the distance calculation to determine how bit the light sphere
		//	object is going to be to draw...

		cLight &Light0 = ::g_pLightManager->vecLights[0];

		// HACK: Sets the light location and attenuation once, then we can move it...
		if ( ! bHACK_RUN_ONCE_TO_SET_LIGHT)
		{
			Light0.position = glm::vec3(0.0f, 100.0f, -180.0f);
			Light0.attenuation.x = 0.0f;
			Light0.attenuation.y = 0.00330822f;
			Light0.attenuation.z = 0.00507976f;
			bHACK_RUN_ONCE_TO_SET_LIGHT = true;
		}
		// This looks nice: 0, 100, -180 dist = 136.719atten: 0:0.00330822:0.00507976


		float HowMuchLightIsEffectivelyDark = 0.01f;		// 1 percent?
		float lightEffectDistance 
			= Light0.calcApproxDistFromAtten(HowMuchLightIsEffectivelyDark,
											 0.001f );		// Within 0.1 percent?

		//  myLight[lightID].typeParams.y is lightDistance
		Light0.typeParams.y = lightEffectDistance;

		g_pLightSphere2Sided->vecMeshes[0].scale = lightEffectDistance;

		// Set this sphere where the light is...
		cPhysProps LightSpherePys = ::g_pLightSphere2Sided->GetPhysState();
		LightSpherePys.position = ::g_pLightManager->vecLights[0].position;

		::g_pLightSphere2Sided->SetPhysState(LightSpherePys);
		std::cout 
			<< Light0.position.x << ", "
			<< Light0.position.y << ", "
			<< Light0.position.z
			<< " dist = " << lightEffectDistance 
			<< "atten: " 
			<< Light0.attenuation.x << ":"
			<< Light0.attenuation.y << ":" 
			<< Light0.attenuation.z << std::endl;

		vecLightAccumulationPass.push_back(::g_pLightSphere2Sided);
		//vecLightAccumulationPass.push_back(::g_pSkyBoxObject);
	//	glUniform( WhichLightIsThisID );
		RenderScene(vecLightAccumulationPass, ::g_pGLFWWindow, deltaTime );




		// Set the scissor buffer
//// Example to render only INSIDE the scissor buffer (square) area
//		glScissor(500, 500, 1080, 600);
//		glEnable(GL_SCISSOR_TEST );
//		RenderScene(vecCopy2ndPass, ::g_pGLFWWindow, deltaTime );
//		glDisable(GL_SCISSOR_TEST );
//// 
//
//// Example to render only OUTSIDE of the scissor area:
//		RenderScene(vecCopy2ndPass, ::g_pGLFWWindow, deltaTime );
//		glScissor(500, 500, 1080, 600);
//		glEnable(GL_SCISSOR_TEST );
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		glDisable(GL_SCISSOR_TEST );
//// 


//    ___  _              _   ___  ___    ___               
//   | __|(_) _ _   __ _ | | |_  )|   \  | _ \ __ _  ___ ___
//   | _| | || ' \ / _` || |  / / | |) | |  _// _` |(_-<(_-<
//   |_|  |_||_||_|\__,_||_| /___||___/  |_|  \__,_|/__//__/
//                                                          	
// Here, we read from the off screen buffer, the one that 
// has all the lighting, etc. 
// This is where we can do the "2nd pass effects", so the 
// full-screen 2D effects.
//
// NOTE: In this example, we are only using this to render to an offscreen object

		// Now the final pass (in this case, only rendering to a quad)
		//RENDER_PASS_2_FULL_SCREEN_EFFECT_PASS
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

		::g_pShaderManager->useShaderProgram("mySexyShader");

		glUniform1i(renderPassNumber_LocID, RENDER_PASS_2_FULL_SCREEN_EFFECT_PASS );

		/// The "deferred pass" FBO has a colour texture with the entire rendered scene
		/// (including lighting, etc.)
		GLint fullRenderedImage2D_LocID = glGetUniformLocation(sexyShaderID, "fullRenderedImage2D");

		/// Pick a texture unit... 
		unsigned int pass2unit = 50;
		glActiveTexture( GL_TEXTURE0 + pass2unit);
		glBindTexture(GL_TEXTURE_2D, ::g_FBO_Pass2_Deferred.colourTexture_0_ID);
		glUniform1i(fullRenderedImage2D_LocID, pass2unit);


		std::vector< cGameObject* >  vecCopySingleLonelyQuad;
		/// Push back a SINGLE quad or GIANT triangle that fills the entire screen
//		vecCopySingleLonelyQuad.push_back( ::g_ExampleTexturedQuad );
//		cGameObject* pTheShip = findObjectByFriendlyName( "NCC-1701", ::g_vecGameObjects );
//		vecCopySingleLonelyQuad.push_back( pTheShip );
		vecCopySingleLonelyQuad.push_back( ::g_pSkyBoxObject );
		RenderScene(vecCopySingleLonelyQuad, ::g_pGLFWWindow, deltaTime);





//		RenderScene(::g_vecTransparentObjectsOnly, ::g_pGLFWWindow, deltaTime);

		std::stringstream ssTitle;
//		ssTitle << "Camera (xyz): " 
//			<< g_cameraXYZ.x << ", " 
//			<< g_cameraXYZ.y << ", " 
//			<< g_cameraXYZ.z;
		// 
//		glm::vec4 EulerAngle;
		//g_pTheCamera->qOrientation.eularAngles(EulerAngle);
		//ssTitle << "Camera (xyz): "
		//	<< EulerAngle.x << ", "
		//	<< EulerAngle.y << ", "
		//	<< EulerAngle.z;

		glfwSetWindowTitle( ::g_pGLFWWindow, ssTitle.str().c_str() );

		// "Presents" what we've drawn
		// Done once per scene 
        glfwSwapBuffers(::g_pGLFWWindow);
        glfwPollEvents();


    }// while ( ! glfwWindowShouldClose(window) )


    glfwDestroyWindow(::g_pGLFWWindow);
    glfwTerminate();

	// 
	delete ::g_pShaderManager;
	delete ::g_pVAOManager;


	::g_pDalekManager->KillAllDaleks();

//    exit(EXIT_SUCCESS);
	return 0;
}


void DrawDebugLightingSpheres(void)
{
	//DEBUG sphere
	::g_pDebugRenderer->addDebugSphere( glm::vec3( 0.0f, 0.0f, 0.0f ), 
										glm::vec4( 0.0f, 1.0f, 0.0f, 1.0f ), 1.0f );
	// Light at 95% 
	float scaleAt99 = ::g_pLightManager->vecLights[0].calcApproxDistFromAtten( 0.99f );	
	::g_pDebugRenderer->addDebugSphere( ::g_pLightManager->vecLights[0].position, 
										glm::vec4( 0.0f, 1.0f, 0.0f, 1.0f ), scaleAt99 );

	// Light at 50% 
	float scaleAt50 = ::g_pLightManager->vecLights[0].calcApproxDistFromAtten( 0.5f );	
	::g_pDebugRenderer->addDebugSphere( ::g_pLightManager->vecLights[0].position, 
										glm::vec4( 0.0f, 1.0f, 0.0f, 1.0f ), scaleAt50 );

	// Light at 25% 
	float scaleAt25 = ::g_pLightManager->vecLights[0].calcApproxDistFromAtten( 0.25f );	
	::g_pDebugRenderer->addDebugSphere( ::g_pLightManager->vecLights[0].position, 
										glm::vec4( 0.0f, 1.0f, 0.0f, 1.0f ), scaleAt25 );

	// Light at 1% 
	float scaleAt01 = ::g_pLightManager->vecLights[0].calcApproxDistFromAtten( 0.01f );	
	::g_pDebugRenderer->addDebugSphere( ::g_pLightManager->vecLights[0].position, 
										glm::vec4( 0.0f, 1.0f, 0.0f, 1.0f ), scaleAt01 );
	return;
}



void setWindowFullScreenOrWindowed( GLFWwindow* pTheWindow, bool IsFullScreen )
{
	// Set full-screen or windowed
	if ( ::g_IsWindowFullScreen )
	{	
		// Find the size of the primary monitor
		GLFWmonitor* pPrimaryScreen = glfwGetPrimaryMonitor();
		const GLFWvidmode* pPrimMonVidMode = glfwGetVideoMode( pPrimaryScreen );
		// Set this window to full screen, matching the size of the monitor:
		glfwSetWindowMonitor( pTheWindow, pPrimaryScreen, 
							  0, 0,				// left, top corner 
							  pPrimMonVidMode->width, pPrimMonVidMode->height, 
							  GLFW_DONT_CARE );	// refresh rate

		std::cout << "Window now fullscreen at ( " 
			<< pPrimMonVidMode->width << " x " 
			<< pPrimMonVidMode->height << " )" << std::endl;
	}
	else
	{
		// Make the screen windowed. (i.e. It's CURRENTLY full-screen)
		// NOTE: We aren't saving the "old" windowed size - you might want to do that...
		// HACK: Instead, we are taking the old size and mutiplying it by 75% 
		// (the thinking is: the full-screen switch always assumes we want the maximum
		//	resolution - see code above - but when we make that maximum size windowed,
		//  it's going to be 'too big' for the screen)
		GLFWmonitor* pPrimaryScreen = glfwGetPrimaryMonitor();
		const GLFWvidmode* pPrimMonVidMode = glfwGetVideoMode( pPrimaryScreen );

		// Put the top, left corner 10% of the size of the full-screen
		int topCornerTop = (int)( (float)pPrimMonVidMode->height * 0.1f );
		int topCornerLeft = (int)( (float)pPrimMonVidMode->width * 0.1f );
		// Make the width and height 75% of the full-screen resolution
		int height = (int)( (float)pPrimMonVidMode->height * 0.75f );
		int width = (int)( (float)pPrimMonVidMode->width * 0.75f );

		glfwSetWindowMonitor( pTheWindow, NULL,		// This NULL makes the screen windowed
							  topCornerLeft, topCornerTop,				// left, top corner 
							  width, height, 
							  GLFW_DONT_CARE );	// refresh rate

		std::cout << "Window now windowed at ( " 
			<< width << " x " << height << " )" 
			<< " and offset to ( "
			<< topCornerLeft << ", " << topCornerTop << " )"
			<< std::endl;
	}
	return;
}
























