#ifndef _cCamera_HG_
#define _cCamera_HG_

#include <glm/vec3.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <string>

class cCamera
{
public:
	cCamera();
	~cCamera();

	// For following, etc. 
	void updateTick(double deltaTime);

	glm::mat4 getViewMatrix(void);

	enum eMode
	{
		MODE_MANUAL,				// Move along the axes (lookat)
		MODE_FOLLOW,				// Follows a target (lookat)
		MODE_FLY_USING_LOOK_AT		// Here, you use the "target" as direction
									// you want to go. This allows you to transition
									// from the FOLLOW_CAMERA to FLY seamlessly
	};

	void setCameraMode(eMode cameraMode);
	eMode getCameraMode(void);
	std::string getCameraModeString(void);

	glm::vec3 getEyePosition(void);

	// These are used to simplify the interface for the programmer, 
	//	breaking the various modes of the camera into groups
	class cManualCameraRedirect
	{
	public:
		void setEyePosition(glm::vec3 newPos);
		void adjustEyePosition(glm::vec3 deltaPos);
		void setTargetInWorld(glm::vec3 worldLocation);
		void adjustTargetInWorld(glm::vec3 deltaWorldLocation);
		void setUpVector(glm::vec3 up);

		glm::vec3 getTargetPosition(void);
		glm::vec3 getUpVector(void);
	private:
		friend cCamera;
		cManualCameraRedirect(cCamera* pTheCamera);
		cManualCameraRedirect();		// Don't call
		cCamera* pParentCamera;
		// called BY camera
		void m_updateTick(double deltaTime);
	};

	class cFollowCameraRedirect
	{
	public:
		void setOrUpdateTarget(glm::vec3 target);
		void setIdealCameraLocation(glm::vec3 relativeToTarget);
		void setMaxFollowSpeed(float speed);
		void setDistanceMaxSpeed(float distanceToTarget);
		void setDistanceZeroSpeed(float distanceToTarget);
		void setAll(glm::vec3 target, glm::vec3 idealCamLocRelToTarget, 
					float maxSpeed, float maxSpeedDistance, float zeroSpeedDistance);
	private:
		friend cCamera;
		cFollowCameraRedirect(cCamera *pTheCamera);
		cFollowCameraRedirect();	// Don't call
		cCamera* pParentCamera;
		// called by camera
		void m_updateTick(double deltaTime);
	};

	// This adjusts the target
	// To set, do one of the following:
	//	- If target matters, set target, then orientation
	//	- If specific target doesn't matter, you can set orientation only
	// Moving:
	//  - If target +IS+ updated, then it flies like a typical camera
	//  - If target is NOT updated, then the camera always flies towards target
	//    ++i.e.++ the orientation WILL CHANGE to align with the target
	class cFlyCameraLookAtRedirect
	{
	public:
		// If true, target is updated when camera moves.
		//	- This is a 'normal' fly camera, where the movement determines the target
		// If false, target stays locked. 
		void setModeUpdateTargetWithMovement(bool bUpdateTargetWithMovement /*default is true*/);
		void setModeKeepCameraAlignedWithUpVector(bool bAlignWithUpVector /*default is true*/);

		// These change the state when the camera is dynamically changing, over multiple frames
		enum eDynamicState
		{
			IS_STATIC,						// normal mode
			IS_TRACKING_TO_NEW_TARGET,
			IS_TRACKING_TO_NEW_ORIENTATION
		};
		void trackToNewTarget(glm::vec3 targetWorldLocation, float maxAnglePerSecond, bool bIsDegrees = true);
		void trackToNewOrientation(glm::vec3 eulerNewOrientation, float maxAnglesPerSecond, bool bIsDegrees = true);
		void trackToNewOrientation(glm::quat qNewOrientation, float maxAnglesPerSecond, bool bIsDegrees = true);

		void setEyePosition(glm::vec3 newPos);
		void setTargetInWorld(glm::vec3 worldLocation);
		void setUpVector(glm::vec3 up);
		// Sets the target relative to where the camera eye is
		void setDirectionRelative(glm::vec3 relativeDirection);
		void setOrientationFromTarget(void);
		void setOrientationFromTarget(glm::vec3 targetWorldLocation);
		//
		void moveForward(float distance);		// void moveBackwards(float distance);
		void moveRight(float distance);			// void moveLeft(float distance);
		void moveUp(float distance);			// void moveDown(float distance);
		void move(glm::vec3 direction_Zforward_Yup_Xright);
		void yawOrTurnRight(float angle, bool isDegrees = true);	// void yawOrTurnLeft(float angle, bool isDegrees = true);
		void pitchUp(float angle, bool isDegrees = true);			// void pitchDown(float angle, bool isDegrees = true);
		void rollClockWise(float angle, bool isDegrees = true);		// void rollCClockWise(float angle, bool isDegrees = true);

	private:
		friend cCamera;
		cFlyCameraLookAtRedirect(cCamera* pTheCamera);
		cFlyCameraLookAtRedirect();		// Don't call
		cCamera* pParentCamera;
		// called by camera
		void m_updateTick(double deltaTime);
		// Specific to the fly camera
		glm::quat m_calcRotationBetweenVector( glm::vec3 start, glm::vec3 destination);
		glm::quat m_quatLookAtWithUp(glm::vec3 direction, glm::vec3 frontVector = glm::vec3(0.0f, 0.0f, 1.0f) /*+z axis default forward*/);
		void m_calcDirectionFromTarget(glm::vec3 &direction, float &distanceToTarget);
		bool m_bUpdateTargetWithMovement;
		bool m_bKeepAlignedWithUpVector;
		// Dynamic state...
		eDynamicState m_curDynamicState;	// = IS_STATIC
	};



	// The public variables for the various modes
	cManualCameraRedirect*		ManualCam;
	cFollowCameraRedirect*		FollowCam;
	cFlyCameraLookAtRedirect*	FlyCamLA;

private:

	eMode m_cameraMode;

	// connect this to the physics updater??
	void m_EulerIntegrate(double deltaTime);

	// State:
	glm::quat m_qOrientation;
	glm::vec3 m_eye;			// position
	glm::vec3 m_target;
	glm::vec3 m_up;
	glm::vec3 m_velocity;		// For "fly camera", like in blender
	glm::vec3 m_accel;			// For "thruster" like with rockets


};

#endif
