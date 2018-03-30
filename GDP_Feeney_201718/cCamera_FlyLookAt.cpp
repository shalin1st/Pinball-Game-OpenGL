#include "cCamera.h"

// Don't call
// This class is only used with the camera, and needs a callback to the "parent" camera object. 
// So you NEVER create it without passing the parent camera.
cCamera::cFlyCameraLookAtRedirect::cFlyCameraLookAtRedirect()
{
	assert(true);
	return;
}

cCamera::cFlyCameraLookAtRedirect::cFlyCameraLookAtRedirect(cCamera* pTheCamera)
{
	this->pParentCamera = pTheCamera;
	this->m_bUpdateTargetWithMovement = true;
	this->m_bKeepAlignedWithUpVector = true;
	return;
}

void cCamera::cFlyCameraLookAtRedirect::setModeUpdateTargetWithMovement(bool bUpdateTarget /*default is true*/)
{
	this->m_bUpdateTargetWithMovement = true;
	return;
}

void cCamera::cFlyCameraLookAtRedirect::setModeKeepCameraAlignedWithUpVector(bool bAlignWithUpVector /*default is true*/)
{
	this->m_bKeepAlignedWithUpVector = bAlignWithUpVector;
	return;
}

void cCamera::cFlyCameraLookAtRedirect::m_calcDirectionFromTarget(glm::vec3 &direction, float &distanceToTarget)
{
	direction = glm::normalize(this->pParentCamera->m_target - this->pParentCamera->m_eye);
	distanceToTarget = glm::distance(this->pParentCamera->m_target, this->pParentCamera->m_eye);
	return;
}

// Taken mainly from: http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-17-quaternions
glm::quat cCamera::cFlyCameraLookAtRedirect::m_calcRotationBetweenVector( glm::vec3 start, glm::vec3 destination)
{
	start = glm::normalize(start);
	destination = glm::normalize(destination);

	float cosTheta = glm::dot(start, destination);
	glm::vec3 rotationAxis;

	if (cosTheta < -1 + 0.001f)
	{
		// special case when vectors in opposite directions:
		// there is no "ideal" rotation axis
		// So guess one; any will do as long as it's perpendicular to start
		rotationAxis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), start);
		if (glm::length2(rotationAxis) < 0.01 ) 
		{	// bad luck, they were parallel, try again!
			rotationAxis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), start);
		}

		rotationAxis = normalize(rotationAxis);
		return glm::angleAxis(glm::radians(180.0f), rotationAxis);
	}

	rotationAxis = glm::cross(start, destination);

	float s = sqrt( (1+cosTheta)*2 );
	float invs = 1 / s;

	return glm::quat(
		s * 0.5f, 
		rotationAxis.x * invs,
		rotationAxis.y * invs,
		rotationAxis.z * invs
	);
}

// Mostly taken from: http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-17-quaternions
glm::quat cCamera::cFlyCameraLookAtRedirect::m_quatLookAtWithUp(glm::vec3 direction, 
                                                                glm::vec3 frontVector /*=glm::vec3(0.0f, 0.0f, 1.0f), +z axis default forward*/)
{
	glm::quat targetOrientation = glm::quat(glm::vec3(0.0f,0.0f,0.0f));

	glm::quat rot1 = this->m_calcRotationBetweenVector(frontVector, direction);

	// Are we updating the "up" vector? 
	if ( ! this->m_bKeepAlignedWithUpVector )
	{
		// No, we are changing the "up" when we rotate
		// HACK: Not 100% sure if this is doing what we want... (updating the up vector)
		glm::vec3 right = glm::cross(direction, this->pParentCamera->m_up);
		this->pParentCamera->m_up = glm::cross(right, direction);

		targetOrientation = rot1;

	}
	else
	{	
		// Fix rotation to match current 'up' vector (as roll orientation is likely turned)
		glm::vec3 newUp = rot1 * this->pParentCamera->m_up;
		glm::quat rot2 = this->m_calcRotationBetweenVector(newUp, this->pParentCamera->m_up);
	
		targetOrientation = rot2 * rot1;

	}//if ( ! this->m_bKeepAlignedWithUpVector )

	return targetOrientation;
}






void cCamera::cFlyCameraLookAtRedirect::m_updateTick(double deltaTime)
{

	return;
}

void cCamera::cFlyCameraLookAtRedirect::setEyePosition(glm::vec3 newPos)
{
	this->pParentCamera->m_eye = newPos;
	return;
}

void cCamera::cFlyCameraLookAtRedirect::setTargetInWorld(glm::vec3 worldLocation)
{
	this->pParentCamera->m_target = worldLocation;
	return;
}

void cCamera::cFlyCameraLookAtRedirect::setUpVector(glm::vec3 up)
{
	this->pParentCamera->m_up = up;
	return;
}


void cCamera::cFlyCameraLookAtRedirect::setDirectionRelative(glm::vec3 relativeDirection)
{
	this->pParentCamera->m_target = this->pParentCamera->m_eye + relativeDirection;
	return;
}

glm::vec3 calcDirectionVector(glm::vec3 target, glm::vec3 eye)
{
	return glm::normalize(target-eye);
}

// From indirection interface
void cCamera::cFlyCameraLookAtRedirect::moveForward(float distance)
{
	this->move(glm::vec3(0.0f,0.0f,distance));
	return;
}

void cCamera::cFlyCameraLookAtRedirect::moveRight(float distance)
{
	this->move(glm::vec3(distance, 0.0f, 0.0f));

	return;
}

void cCamera::cFlyCameraLookAtRedirect::moveUp(float distance)
{
	this->move(glm::vec3(0.0f, distance, 0.0f));
	return;
}

void cCamera::cFlyCameraLookAtRedirect::move(glm::vec3 direction_Zforward_Yup_Xright)
{
	//// For forward direction
	//glm::vec3 direction = calcDirectionVector(this->pParentCamera->m_target, this->pParentCamera->m_eye);

	//// For right left (perpendicular from direction and up)
	//glm::vec3 dirLeftRight = glm::normalize(glm::cross(direction, this->pParentCamera->m_up));
	//
	//// Move "up" relative to camera, not the "up" vector (which is just where "up" is in the world)
	//glm::vec3 dirUpDown = glm::normalize(glm::cross(direction, dirLeftRight));

	//// Forward is based on direction
	//direction_Zforward_Yup_Xright.z = direction_Zforward_Yup_Xright.z * direction.z;
	//// Left-right...
	//direction_Zforward_Yup_Xright.x = direction_Zforward_Yup_Xright.x * direction.x;
	//// Up-down...
	//direction_Zforward_Yup_Xright.y = direction_Zforward_Yup_Xright.y * direction.y;

	//this->pParentCamera->m_eye += direction_Zforward_Yup_Xright;
	//this->pParentCamera->m_target += direction_Zforward_Yup_Xright;


	glm::vec3 direction = glm::vec3(0.0f);
	float distanceToTarget = 0.0f;
	this->m_calcDirectionFromTarget(direction, distanceToTarget);

	glm::quat qOrientation = this->m_quatLookAtWithUp(direction, glm::vec3(0.0f, 0.0f, 1.0f));

	glm::mat3 matRotation(qOrientation);

	glm::vec3 deltaMove = matRotation * direction_Zforward_Yup_Xright;

	this->pParentCamera->m_eye += deltaMove;
	if ( this->m_bUpdateTargetWithMovement)
	{
		this->pParentCamera->m_target += deltaMove;
	}

	return;
}

void cCamera::cFlyCameraLookAtRedirect::yawOrTurnRight(float angle, bool isDegrees /*=true*/)
{
	glm::vec3 direction = calcDirectionVector(this->pParentCamera->m_target, this->pParentCamera->m_eye);
	return;
}

void cCamera::cFlyCameraLookAtRedirect::pitchUp(float angle, bool isDegrees /*=true*/)
{
	//TODO
	return;
}

void cCamera::cFlyCameraLookAtRedirect::rollClockWise(float angle, bool isDegrees /*=true*/)
{
	//TODO
	return;
}


//
//void cCamera::m_calcDirectionVectorFromTarget(glm::vec3 &eyeToTargetDirection, float &eyeToTargetLength)
//{
//	// Get a vector from the target to the eye
//	// (i.e. what direction the camera is looking at)
//	glm::vec3 vDirectionAndDistanceTarget = this->target - this->eye;
//	
//	eyeToTargetLength = glm::length(vDirectionAndDistanceTarget);
//	// Scale this to 1.0 (normalize)
//	eyeToTargetDirection = glm::normalize(vDirectionAndDistanceTarget);
//}
//
//void cCamera::m_calcDirectionVectorFromTarget(glm::vec3 &vEyeToTargetDirection)
//{
//	float eyeToTargetLength = 0.0f;
//	return this->m_calcDirectionVectorFromTarget( vEyeToTargetDirection, eyeToTargetLength );
//}
//
//
//
//void cCamera::Fly_moveForward(float distanceAlongRelativeZAxis_PosIsForward)
//{
//	// Calcualte axis components based on orientation
//	glm::vec3 vDirection;
//	this->m_calcDirectionVectorFromTarget(vDirection);
//
//	// Scale the direction we want to go towards the direction the camera is looking at
//	glm::vec3 finalDeltaPosition = vDirection * distanceAlongRelativeZAxis_PosIsForward;
//
//	// Update the position
//	this->eye += finalDeltaPosition;
//
//	return;
//}
//
//void cCamera::Fly_moveRightLeft(float distanceAlongRelativeXAxis_PosIsRight)
//{
//	// Calcualte axis components based on orientation
//	glm::vec3 vDirection;
//	this->m_calcDirectionVectorFromTarget(vDirection);
//
//	// Scale the direction we want to go towards the direction the camera is looking at
//	glm::vec3 moveDelta = glm::vec3( distanceAlongRelativeXAxis_PosIsRight, 0.0f, 0.0f );
//	glm::vec3 finalDeltaPosition = vDirection * moveDelta;
//
//	// Update the position
//	this->eye += finalDeltaPosition;
//
//	return;
//}
//
//void cCamera::Fly_moveUpDown(float distanceAlongRelativeYAxis_PosIsUp)
//{
//	// Calcualte axis components based on orientation
//	glm::vec3 vDirection;
//	this->m_calcDirectionVectorFromTarget(vDirection);
//
//	// Scale the direction we want to go towards the direction the camera is looking at
//	glm::vec3 moveDelta = glm::vec3( 0.0f, distanceAlongRelativeYAxis_PosIsUp, 0.0f );
//	glm::vec3 finalDeltaPosition = vDirection * moveDelta;
//
//	// Update the position
//	this->eye += finalDeltaPosition;
//
//	return;
//}
//// +ve is right
//void cCamera::Fly_turn_RightLeft(float turnDegreesPosIsRight)
//{
//	// Get direction vector and length
//	glm::vec3 vEyeToTargetDirection;
//	float eyeToTargetLength = 0.0f;
//	this->m_calcDirectionVectorFromTarget(vEyeToTargetDirection, eyeToTargetLength);
//
//	// Convert vector into a quaternion
//	glm::quat qAdjust = glm::quat(vEyeToTargetDirection);
//
//	// Rotate with quaternion
//
//	// Scale target back to original location
//
//	//TODO:
//	return;
//}
//
//// +ve it up
//void cCamera::Fly_pitch_UpDown(float pitchDegreesPosIsNoseUp)
//{
//	//TODO:
//	return;
//}
//// +ve is Clock-wise rotation (from nose to tail)
//void cCamera::Fly_yaw_CWorCCW(float pitchDegreesPosIsClockWise)
//{
//	//TODO:
//	return;
//}
//// 
//// You can use this to change the target from wherever it is
////	to, say, 1.0 units from the front of the camera. 
//void cCamera::RelocateTargetInFrontOfCamera(float howFarAwayFromFront)
//{
//	//TODO:
//	return;
//}


////******************************************************************
//// Below is the "garbage" version of the camera. 
//// Please don't use it...
//// ************************************************************
//// For the "fly camera":
//// +ve is along z-axis
//void cCamera::Fly_moveForward(float distanceAlongRelativeZAxis_PosIsForward)
//{
//	// Along the z axis
//	this->eye.z += distanceAlongRelativeZAxis_PosIsForward;
//	return;
//}
//
//void cCamera::Fly_moveRightLeft(float distanceAlongRelativeXAxis_PosIsRight)
//{
//	// Along the x axis
//	this->eye.x += distanceAlongRelativeXAxis_PosIsRight;
//	return;
//}
//
//void cCamera::Fly_moveUpDown(float distanceAlongRelativeYAxis_PosIsUp)
//{
//	// Along the y
//	this->eye.y += distanceAlongRelativeYAxis_PosIsUp;
//	return;
//}
//
//// +ve is right
//void cCamera::Fly_turn_RightLeft(float turnDegreesPosIsRight)
//{
//	// From camera orientation, Y axis "turns" left and right
//	this->adjustQOrientationFormDeltaEuler(
//		glm::vec3(0.0f,
//		glm::radians(turnDegreesPosIsRight),	// Y axis
//		0.0f));
//	return;
//}
//// +ve it up
//void cCamera::Fly_pitch_UpDown(float pitchDegreesPosIsNoseUp)
//{
//	// From camera orientation, X axis "pitches" up and down
//	this->adjustQOrientationFormDeltaEuler(
//		glm::vec3(glm::radians(pitchDegreesPosIsNoseUp),
//		0.0f,
//		0.0f));
//	return;
//}
//
//// +ve is Clock-wise rotation (from nose to tail)
//void cCamera::Fly_yaw_CWorCCW(float pitchDegreesPosIsClockWise)
//{
//	// From camera orientation, Z axis "rolls" around
//	// (assume z axis is "nose to tail"
//	this->adjustQOrientationFormDeltaEuler(
//		glm::vec3(0.0f,
//		0.0f,
//		glm::radians(pitchDegreesPosIsClockWise)));
//	return;
//}
////******************************************************************
