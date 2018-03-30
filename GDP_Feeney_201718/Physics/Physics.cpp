#error "This file has been replaced with the cPhysicsWorld class"

/*

	This entire thing is replaced with the cPhysicsWorld class


#include "Physics.h"
#include "../globalGameStuff.h"

#include <glm/glm.hpp>		// glm::distance

bool PenetrationTestSphereSphere( cGameObject* pA, cGameObject* pB )
{
	// If the distance between the two sphere is LT the sum or the radii,
	//	they are touching or penetrating

	float totalRadii = pA->radius + pB->radius;

	// The Pythagorean distance 
	float distance = glm::distance( pA->position, pB->position );

	if ( distance <= totalRadii )
	{
		return true;
	}

	return false;
}

// HACK plane collision detection and response
const float SURFACEOFGROUND = -10.0f;
const float RIGHTSIDEWALL = 15.0f;
const float LEFTSIDEWALL = -15.0f;

// Update the world 1 "step" in time
void PhysicsStep( double deltaTime )
{
	// Distance                          m
	// Velocity = distance / time		 m/s
	// Accleration = velocity / time     m/s/s

	// Distance = time * velocity
	// velocity = time * acceleration

	//// HACK: Change all objects back to white (before collision test)
	//for ( int index = 0; index != ::g_vecGameObjects.size(); index++ )
	//{
	//	cGameObject* pCurGO = ::g_vecGameObjects[index];
	//	pCurGO->diffuseColour = glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f );
	//}//for ( int index


	const glm::vec3 GRAVITY = glm::vec3(0.0f, -2.0f, 0.0f);

	// Identical to the 'render' (drawing) loop
	for ( int index = 0; index != ::g_vecGameObjects.size(); index++ )
	{
		cGameObject* pCurGO = ::g_vecGameObjects[index];

		// Is this object to be updated?
		if ( ! pCurGO->bIsUpdatedInPhysics )
		{	// DON'T update this
			continue;		// Skip everything else in the for
		}

		// Explicit Euler  (RK4) 
		// New position is based on velocity over time
		glm::vec3 deltaPosition = (float)deltaTime * pCurGO->vel;
		pCurGO->position += deltaPosition;

		// New velocity is based on acceleration over time
		glm::vec3 deltaVelocity =  ( (float)deltaTime * pCurGO->accel )
			                     + ( (float)deltaTime * GRAVITY );

		pCurGO->vel += deltaVelocity;


		// HACK: Collision step
		// Assume the "ground" is -2.0f from the origin in the y
		switch ( pCurGO->typeOfObject )
		{
		case eTypeOfObject::SPHERE:	
			// Comare this to EVERY OTHER object in the scene
			for ( int indexEO = 0; indexEO != ::g_vecGameObjects.size(); indexEO++ )
			{
				// Don't test for myself
				if ( index == indexEO )		
					continue;	// It's me!! 

				cGameObject* pOtherObject = ::g_vecGameObjects[indexEO];
				// Is Another object
				switch ( pOtherObject->typeOfObject )
				{
				case eTypeOfObject::SPHERE:
					// 
					if ( PenetrationTestSphereSphere( pCurGO, pOtherObject ) )
					{
						////std::cout << "Collision!" << std::endl;
						//pCurGO->diffuseColour = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
						//pOtherObject->diffuseColour = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
					}
					break;

				}//switch ( pOtherObject->typeOfObject )

			}//for ( int index
		//	{
		//		switch ( pGO_to_Compare->typeOfObject )
		//		{
		//		case eTypeOfObject::SPHERE:
		//			CalcSphereSphereColision( pCurGO, pGO_to_Compare );
		//			break;
		//		case eTypeOfObject::PLANE:
		//			CalcSpherePlaneColision( pCurGO, pGO_to_Compare );
		//			break;
		//		// More if I'd like that.
		//
		//		}
		//	}

			// HACK
			//const float SURFACEOFGROUND = -2.0f;
			//const float RIGHTSIDEWALL = 5.0f;
			//const float LEFTSIDEWALL = -5.0f;
			// 
//			float x = 3.0f;

			// Sphere-Plane detection

			if ( ( pCurGO->position.y - pCurGO->radius ) <= SURFACEOFGROUND )	// y=-2
			{	// Object has "hit" the ground
//				pCurGO->diffuseColour = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
				pCurGO->vel.y = +( fabs(pCurGO->vel.y) );
			}

			if ( ( pCurGO->position.x + pCurGO->radius ) >= RIGHTSIDEWALL )
			{	// Object too far to the right
				// Object has penetrated the right plane
//				pCurGO->diffuseColour = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
				pCurGO->vel.x = -( fabs(pCurGO->vel.x) );
			}
			if ( ( pCurGO->position.x - pCurGO->radius ) <= LEFTSIDEWALL )
			{	// Object too far to the left
				// Object has penetrated the left plane
//				pCurGO->diffuseColour = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
				pCurGO->vel.x = +( fabs(pCurGO->vel.x) );
			}
			break;
		};





	}//for ( int index...



	return;
}


*/
