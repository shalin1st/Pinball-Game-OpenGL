#include "globalOpenGL_GLFW.h"
#include "globalGameStuff.h"

#include <iostream>

#include "cAnimationState.h"

bool isShiftKeyDown( int mods, bool bByItself = true );
bool isCtrlKeyDown( int mods, bool bByItself = true );
bool isAltKeyDown( int mods, bool bByItself = true );
bool areAllModifierKeysUp(int mods);
bool areAnyModifierKeysDown(int mods);

extern std::string g_AnimationToPlay;

extern double g_CA_CountDown;// = 0.0f;

/*static*/ void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);


	// Fullscreen to windowed mode on the PRIMARY monitor (whatever that is)
	if ( isAltKeyDown(mods, true) && key == GLFW_KEY_ENTER )
	{
		if ( action == GLFW_PRESS )
		{
			::g_IsWindowFullScreen = !::g_IsWindowFullScreen;

			setWindowFullScreenOrWindowed( ::g_pGLFWWindow, ::g_IsWindowFullScreen );

		}//if ( action == GLFW_PRESS )
	}//if ( isAltKeyDown(...


	cGameObject* pLeftTeapot = findObjectByFriendlyName(LEFTTEAPOTNAME, ::g_vecGameObjects);

	cGameObject* pSophie = findObjectByFriendlyName( "Sophie", ::g_vecGameObjects );


	const float CAMERASPEED = 10.0f;

	const float CAM_ACCELL_THRUST = 100.0f;

	if ( isShiftKeyDown(mods, true)  )
	{
		switch (key)
		{
		case GLFW_KEY_1:
			::g_pLightManager->vecLights[0].attenuation.y *= 0.99f;	// less 1%
			break;
		case GLFW_KEY_2:
			::g_pLightManager->vecLights[0].attenuation.y *= 1.01f; // more 1%
			if (::g_pLightManager->vecLights[0].attenuation.y <= 0.0f)
			{
				::g_pLightManager->vecLights[0].attenuation.y = 0.001f;	// Some really tiny value
			}
			break;
		case GLFW_KEY_3:	// Quad
			::g_pLightManager->vecLights[0].attenuation.z *= 0.99f;	// less 1%
			break;
		case GLFW_KEY_4:	//  Quad
			::g_pLightManager->vecLights[0].attenuation.z *= 1.01f; // more 1%
			if (::g_pLightManager->vecLights[0].attenuation.z <= 0.0f)
			{
				::g_pLightManager->vecLights[0].attenuation.z = 0.001f;	// Some really tiny value
			}
			break;

		// Lights
		// CAMERA and lighting
		case GLFW_KEY_A:		// Left
			::g_pLightManager->vecLights[0].position.x -= CAMERASPEED;
			break;
		case GLFW_KEY_D:		// Right
			::g_pLightManager->vecLights[0].position.x += CAMERASPEED;
			break;
		case GLFW_KEY_W:		// Forward (along z)
			::g_pLightManager->vecLights[0].position.z += CAMERASPEED;
			break;
		case GLFW_KEY_S:		// Backwards (along z)
			::g_pLightManager->vecLights[0].position.z -= CAMERASPEED;
			break;
		case GLFW_KEY_Q:		// "Down" (along y axis)
			::g_pLightManager->vecLights[0].position.y -= CAMERASPEED;
			break;
		case GLFW_KEY_E:		// "Up" (along y axis)
			::g_pLightManager->vecLights[0].position.y += CAMERASPEED;
			break;
		case GLFW_KEY_G:
			{
				float angle = ::g_pLightManager->vecLights[0].getLightParamSpotPrenumAngleOuter();
				::g_pLightManager->vecLights[0].setLightParamSpotPrenumAngleOuter(angle + 0.01f);
			}
			break;
		case GLFW_KEY_H:
			{
				float angle = ::g_pLightManager->vecLights[0].getLightParamSpotPrenumAngleOuter();
				::g_pLightManager->vecLights[0].setLightParamSpotPrenumAngleOuter(angle - 0.01f);
			}
			break;
		};//switch (key)
	}//if ( isShiftKeyDown(mods, true) )


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
	//jumpAnimation.totalTime = pTempGO->pSimpleSkinnedMesh->FindAnimationTotalTime(jumpAnimation.name);
	jumpAnimation.totalTime = 1.0f;
	jumpAnimation.frameStepTime = 0.001f;


	if (areAllModifierKeysUp(mods)  )
	{
		//	const float CAMERASPEED = 100.0f;
		switch (key)
		{
		case GLFW_KEY_1:
			::g_pTheCamera->setCameraMode(cCamera::MODE_FLY_USING_LOOK_AT);
			std::cout << "Camera now in " << ::g_pTheCamera->getCameraModeString() << std::endl;;
			break;
		case GLFW_KEY_2:
			::g_pTheCamera->setCameraMode(cCamera::MODE_FOLLOW);
			std::cout << "Camera now in " << ::g_pTheCamera->getCameraModeString() << std::endl;;
			break;
		case GLFW_KEY_3:
			::g_pTheCamera->setCameraMode(cCamera::MODE_MANUAL);
			std::cout << "Camera now in " << ::g_pTheCamera->getCameraModeString() << std::endl;;
			break;

		// CAMERA and lighting
		case GLFW_KEY_A:		// Left
//			::g_pTheCamera->FlyCamLA->moveRight(-CAMERASPEED);	// strafe
			::g_pTheCamera->FlyCamLA->move(glm::vec3(-CAMERASPEED, 0.0f, 0.0f));
			break;
		case GLFW_KEY_D:		// Right
//			::g_pTheCamera->FlyCamLA->moveRight(+CAMERASPEED);	// strafe
			::g_pTheCamera->FlyCamLA->move(glm::vec3(+CAMERASPEED, 0.0f, 0.0f));
			break;
		case GLFW_KEY_W:		// Forward (along z)
//			::g_pTheCamera->FlyCamLA->moveForward(+CAMERASPEED);
			::g_pTheCamera->FlyCamLA->move(glm::vec3(0.0f, 0.0f, +CAMERASPEED));
			break;
		case GLFW_KEY_S:		// Backwards (along z)
//			::g_pTheCamera->FlyCamLA->moveForward(-CAMERASPEED);		// Backwards
			::g_pTheCamera->FlyCamLA->move(glm::vec3(0.0f, 0.0f, -CAMERASPEED));		// Backwards
			break;
		case GLFW_KEY_Q:		// "Down" (along y axis)
//			::g_pTheCamera->FlyCamLA->moveUp(-CAMERASPEED);	// "Z minus 10000 meters, Mr. Sulu!"
			::g_pTheCamera->FlyCamLA->move(glm::vec3(0.0f, -CAMERASPEED, 0.0f));		// Backwards
			break;
		case GLFW_KEY_E:		// "Up" (along y axis)
//			::g_pTheCamera->FlyCamLA->moveUp(+CAMERASPEED);	// "Z minus 10000 meters, Mr. Sulu!"
			::g_pTheCamera->FlyCamLA->move(glm::vec3(0.0f, +CAMERASPEED, 0.0f));		// Backwards
			break;

		case GLFW_KEY_5:
			break;
		case GLFW_KEY_6:
			break;
		case GLFW_KEY_7:
			break;
		case GLFW_KEY_8:
			break;
		case GLFW_KEY_9:
			break;
		case GLFW_KEY_0:
			::g_CA_CountDown = 0.5f;
			break;

		case GLFW_KEY_N:
			break;

		case GLFW_KEY_J:	// Left
			if ( action == GLFW_PRESS )
			{
				//pSophie->pAniState->defaultAnimation.name = "assets/modelsFBX/RPG-Character_Unarmed-Roll-Left(FBX2013).FBX";
				pSophie->pAniState->vecAnimationQueue.push_back( leftAnimation );
			}
			break;
		case GLFW_KEY_L:	// Right
			if ( action == GLFW_PRESS )
			{
				//pSophie->pAniState->defaultAnimation.name = "assets/modelsFBX/RPG-Character_Unarmed-Roll-Right(FBX2013).FBX";
				pSophie->pAniState->vecAnimationQueue.push_back( rightAnimation );
			}
			break;
		case GLFW_KEY_I:	// Back;
//			pSophie->pAniState->defaultAnimation.name = "assets/modelsFBX/RPG-Character_Unarmed-Roll-Backward(FBX2013).fbx";
			break;
		case GLFW_KEY_K:	// Forward:
//			pSophie->pAniState->defaultAnimation.name = "assets/modelsFBX/RPG-Character_Unarmed-Roll-Forward(FBX2013).FBX";
			break;
		case GLFW_KEY_M:	// Walk
//			pSophie->pAniState->defaultAnimation.name = "assets/modelsFBX/RPG-Character_Unarmed-Walk(FBX2013).FBX";
			break;
		case GLFW_KEY_SPACE:	// Jump
			if ( action == GLFW_PRESS )
			{
				//pSophie->pAniState->defaultAnimation.name = "assets/modelsFBX/RPG-Character_Unarmed-Jump(FBX2013).FBX";
				pSophie->pAniState->vecAnimationQueue.push_back( jumpAnimation );
			}

			break;


		}//switch
	}//if (areAllModifierKeysUp(mods))

	// HACK: print output to the console
//	std::cout << "Light[0] linear atten: "
//		<< ::g_pLightManager->vecLights[0].attenuation.y << ", "
//		<< ::g_pLightManager->vecLights[0].attenuation.z << std::endl;
	return;
}



// Helper functions
bool isShiftKeyDown( int mods, bool bByItself /*=true*/ )
{
	if ( bByItself )
	{	// shift by itself?
		if ( mods == GLFW_MOD_SHIFT )	{ return true; }
		else							{ return false; }
	}
	else
	{	// shift with anything else, too
		if ( ( mods & GLFW_MOD_SHIFT ) == GLFW_MOD_SHIFT )	{	return true;	}
		else												{	return false;	}
	}
	// Shouldn't never get here, so return false? I guess?
	return false;
}

bool isCtrlKeyDown( int mods, bool bByItself /*=true*/ )
{
	if ( bByItself )
	{	// shift by itself?
		if ( mods == GLFW_MOD_CONTROL )	{ return true; }
		else							{ return false; }
	}
	else
	{	// shift with anything else, too
		if ( ( mods & GLFW_MOD_CONTROL) == GLFW_MOD_CONTROL)	{	return true;	}
		else												{	return false;	}
	}
	// Shouldn't never get here, so return false? I guess?
	return false;
}

bool isAltKeyDown( int mods, bool bByItself /*=true*/ )
{
	if (bByItself)
	{	// shift by itself?
		if (mods == GLFW_MOD_ALT) { return true; }
		else { return false; }
	}
	else
	{	// shift with anything else, too
		if ((mods & GLFW_MOD_ALT) == GLFW_MOD_ALT) { return true; }
		else { return false; }
	}
	// Shouldn't never get here, so return false? I guess?
	return false;
}

bool areAllModifierKeysUp(int mods)
{
	if ( isShiftKeyDown(mods) )	{	return false;	}
	if ( isCtrlKeyDown(mods) ) 	{	return false;	}
	if ( isAltKeyDown(mods) )	{	return false;	}

	// All of them are up
	return true;
}//areAllModifierKeysUp()

bool areAnyModifierKeysDown(int mods)
{
	if ( isShiftKeyDown(mods) )		{	return true;	}
	if ( isCtrlKeyDown(mods) )		{	return true;	}
	if ( isAltKeyDown(mods) )		{	return true;	}
	// None of them are down
	return false;
}