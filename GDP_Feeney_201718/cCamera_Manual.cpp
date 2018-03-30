#include "cCamera.h"

//glm::vec4 corner;
//corner = matWorld * corner;			// World space

//corner = matInvTranp * corner;		// object space

// Don't call
// This class is only used with the camera, and needs a callback to the "parent" camera object. 
// So you NEVER create it without passing the parent camera.
cCamera::cManualCameraRedirect::cManualCameraRedirect()
{
	assert(true);
	return;
}

cCamera::cManualCameraRedirect::cManualCameraRedirect(cCamera* pTheCamera)
{
	this->pParentCamera = pTheCamera;
	return;
}



void cCamera::cManualCameraRedirect::setEyePosition(glm::vec3 newPos)
{
	this->pParentCamera->m_eye = newPos;
	return;
}

void cCamera::cManualCameraRedirect::adjustEyePosition(glm::vec3 deltaPos)
{
	this->pParentCamera->m_eye += deltaPos;
	return;
}

void cCamera::cManualCameraRedirect::setTargetInWorld(glm::vec3 worldLocation)
{
	this->pParentCamera->m_target = worldLocation;
	return;
}

void cCamera::cManualCameraRedirect::adjustTargetInWorld(glm::vec3 deltaWorldLocation)
{
	this->pParentCamera->m_target += deltaWorldLocation;
	return;
}

void cCamera::cManualCameraRedirect::setUpVector(glm::vec3 up)
{
	this->pParentCamera->m_up = up;
	return;
}


glm::vec3 cCamera::cManualCameraRedirect::getTargetPosition(void)
{
	return this->pParentCamera->m_target;
}

glm::vec3 cCamera::cManualCameraRedirect::getUpVector(void)
{
	return this->pParentCamera->m_up;
}

void cCamera::cManualCameraRedirect::m_updateTick(double deltaTime)
{
	//
	return;
}
