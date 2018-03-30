#include "cCamera.h"
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective


cCamera::cCamera()
{
	// Default OpenGL 1.1 was on z axis looking at the origin
	this->m_eye = glm::vec3(0.0f, 0.0f, 1.0f);
	this->m_target = glm::vec3(0.0f, 0.0f, 0.0f);
	this->m_up = glm::vec3(0.0f, 1.0f, 0.0f);

	// Set quaternion to some default rotation value
	this->m_qOrientation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));

	this->m_accel = glm::vec3(0.0f);
	this->m_velocity = glm::vec3(0.0f);

	// Create the "redirection" classes
	this->FollowCam = new cFollowCameraRedirect(this);
	this->FlyCamLA = new cFlyCameraLookAtRedirect(this);
	this->ManualCam = new cManualCameraRedirect(this);

	return;
}

cCamera::~cCamera()
{
	delete this->FollowCam;
	delete this->FlyCamLA;
	delete this->ManualCam;
	return;
}

glm::vec3 cCamera::getEyePosition(void)
{
	return this->m_eye;
}


// For following, etc. 
void cCamera::updateTick(double deltaTime)
{
	switch (this->m_cameraMode)
	{
	case cCamera::eMode::MODE_MANUAL:
		this->ManualCam->m_updateTick(deltaTime);
		break;
	case cCamera::eMode::MODE_FLY_USING_LOOK_AT:
		this->FlyCamLA->m_updateTick(deltaTime);
		break;
	case cCamera::eMode::MODE_FOLLOW:
		this->m_EulerIntegrate(deltaTime);
		this->FollowCam->m_updateTick(deltaTime);
		break;
	}

	return;
}

void cCamera::setCameraMode(eMode cameraMode)
{
	// Yes, it's an enum, but you can pass anything, so double-check
	switch (cameraMode)
	{
	case cCamera::eMode::MODE_FLY_USING_LOOK_AT:
	case cCamera::eMode::MODE_FOLLOW:
	case cCamera::eMode::MODE_MANUAL:
		this->m_cameraMode = cameraMode;
		return;
	}

	// Something is wrong, so set to manual
	this->m_cameraMode = cCamera::eMode::MODE_MANUAL;
	return;
}

cCamera::eMode cCamera::getCameraMode(void)
{
	return this->m_cameraMode;
}

std::string cCamera::getCameraModeString(void)
{
	switch (this->m_cameraMode)
	{
	case cCamera::MODE_MANUAL:
		return "MANUAL";
		break;
	case cCamera::eMode::MODE_FLY_USING_LOOK_AT:
		return "FLY_CAMERA_USING_LOOK_AT";
		break;
	case cCamera::eMode::MODE_FOLLOW:
		return "FOLLOW_CAMERA";
		break;
	default:
		return "Unkown camera mode";
		break;
	}
	return "Unknown camera mode";
}


glm::mat4 cCamera::getViewMatrix(void)
{
	// Based on the mode, calculate the view matrix
	switch (m_cameraMode)
	{
	case cCamera::eMode::MODE_MANUAL:
		// Use LookAT
		glm::mat4 matView = glm::lookAt(this->m_eye,
										this->m_target,
										this->m_up ); // UP
		return matView;
		break;
	case cCamera::eMode::MODE_FOLLOW:
		{
			glm::mat4 matView = glm::lookAt(this->m_eye,
											this->m_target,
											this->m_up ); // UP
			return matView;
		}
		break;
	case cCamera::eMode::MODE_FLY_USING_LOOK_AT:
		{
			glm::mat4 matView = glm::lookAt(this->m_eye,
											this->m_target,
											this->m_up); // UP
			return matView;
		}
		break;
	}
	// ****************************************************
	// Invalid camera mode, so return the identity matrix
	return glm::mat4(1.0f);
}

void cCamera::m_EulerIntegrate(double deltaTime)
{
	//TODO: Add this, or connect it to the physics updater


	return;
}

////////// ************************************************************
////////
////////void cCamera::setOrientationFromEuler(glm::vec3 eulerAngles);
////////glm::mat4 cCamera::getMat4FromOrientation(void);
//////
//////void cCamera::overwrtiteQOrientationFormEuler(glm::vec3 eulerAxisOrientation)
//////{
//////	// Calcualte the quaternion represnetaiton of this Euler axis
//////	// NOTE: We are OVERWRITING this..
//////	this->qOrientation = glm::quat(eulerAxisOrientation);
//////
//////	return;
//////}
//////
//////void cCamera::adjustQOrientationFormDeltaEuler(glm::vec3 eulerAxisOrientChange)
//////{
//////	// How do we combine two matrices?
//////	// That's also how we combine quaternions...
//////
//////	// So we want to "add" this change in oriention
//////	glm::quat qRotationChange = glm::quat(eulerAxisOrientChange);
//////
//////	// Mulitply it by the current orientation;
//////	this->qOrientation = this->qOrientation * qRotationChange;
//////
//////	return;
//////}
