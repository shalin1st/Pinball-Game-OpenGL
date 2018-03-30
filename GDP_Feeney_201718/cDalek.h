#ifndef _DALEK_HG_
#define _DALEK_HG_

#include "CHRTimer.h"
#include <glm/glm.hpp>

#include "iDalekManager.h"

class cRandThreaded;		// In Utilities.h

// The Daleks look after themselves in terms of calculation,
// moving, etc. 
// BUT, they update the updated position to the Dalek Manager
// Also, they get all the positions of every other Dalek
// from the Dalek Manager

#include "DalekThreadedCommon.h"

class cDalek
{
public:
	cDalek(unsigned int m_DalekID_or_Index);

	// This object is used to access all the other Daleks
	//	and to update +this+ Dalek's location
	void setDalkeManager( iDalekManager* pDM );
	void setRandThreaded( cRandThreaded* pRT );

//	CRITICAL_SECTION CS_POSITION;

	glm::vec3 position;

//	static const unsigned int NUMBER_OF_BUFFERS = 4;
//	glm::vec3 position[NUMBER_OF_BUFFERS];

	unsigned int m_CurrentReadBufferID = 0;
	unsigned int m_CurrentWriteBufferID = 2;

	//glm::vec3 getPosition()
	//{
	//	glm::vec3 thePosition;
	//	EnterCriticalSection( CS_POSITION );
	//	thePosition = this->position;
	//	LeaveCriticalSection( CS_POSITION );
	//	return thePosition;
	//}

	//void setPosition(glm::vec3 newPosition)
	//{
	//	EnterCriticalSection( CS_POSITION );
	//	this->position = newPosition;
	//	LeaveCriticalSection( CS_POSITION );
	//	return;
	//}
	glm::vec3 velocity;
	float maxVelocity;
	glm::vec3 target;
	float timeToMove;

	float minDistanceToMove;
	float maxDistanceToMove;
	static const float DEFAULT_MIN_DISTANCE_TO_MOVE;// = 1.0;
	static const float DEFAULT_MAX_DISTANCE_TO_MOVE;// = 25.0;

	// If false, thread exits
	bool bIsAlive;

	// If true, thread is alive, but not updating
	// Starts as false
	bool bIsUpdating;

	void Update(void);

	// Thread information  (in case we need it)
	sDalekThreadInfo threadInfo;

	unsigned int getDalekID(void);

private:
	cDalek();		// Can't call - is private

	CHRTimer* m_pTimer;

	iDalekManager* m_pDalekManager;
	cRandThreaded* m_pRand;

	// This is the unique or, which is used as the index into the 
	//	vector in the DalekManager. 
	unsigned int m_DalekID_or_Index;

	std::vector<glm::vec3> m_vecDalekPositions;

};

#endif
