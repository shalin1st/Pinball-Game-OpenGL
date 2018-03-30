#include "cDalek.h"
#include "Utilities.h"	// cRandThreaded

#include <iostream>

//#define DEBUG_PRINT_STATUS_TO_CONSOLE

// Can't call (is private)
cDalek::cDalek()
{
	return;
}

// These are all static
const float cDalek::DEFAULT_MIN_DISTANCE_TO_MOVE = 1.0;
const float cDalek::DEFAULT_MAX_DISTANCE_TO_MOVE = 25.0;


cDalek::cDalek(unsigned int DalekID_or_Index)
{
#ifdef DEBUG_PRINT_STATUS_TO_CONSOLE 
	std::cout << "Dalek " << DalekID_or_Index << " is created" << std::endl;
	std::cout.flush();
#endif

	this->position = glm::vec3(0.0f);
	this->velocity = glm::vec3(0.0f);
	this->target = glm::vec3(0.0f);

	// set this to -ve to force it choosing a new direction
	this->timeToMove = -1.0f;

	this->maxVelocity = 1.0f;

	this->bIsAlive = true;

	this->bIsUpdating = false;

	float minDistanceToMove = cDalek::DEFAULT_MIN_DISTANCE_TO_MOVE;
	float maxDistanceToMove = cDalek::DEFAULT_MAX_DISTANCE_TO_MOVE;


	this->m_DalekID_or_Index = DalekID_or_Index;

	// Each Dalek has it's own timer, since each thread could execute at different rates
	this->m_pTimer = new CHRTimer();

	this->m_pTimer->ResetLongDuration();

	this->m_pDalekManager = NULL;
	this->m_pRand = NULL;

	// Thread information (in case we need it)
	HANDLE threadHandle = 0;
	DWORD threadAddress = 0;


	return;
}

unsigned int cDalek::getDalekID(void)
{
	return this->m_DalekID_or_Index;
}


void cDalek::Update(void)
{
#ifdef DEBUG_PRINT_STATUS_TO_CONSOLE 
	std::cout << "Dalek " << this->m_DalekID_or_Index << " Update() called" << std::endl;
	std::cout.flush();
#endif

	float totalTimeSinceReset = this->m_pTimer->get_fLondDurationTotalSeconds();

	if ( this->m_pTimer->get_fLondDurationTotalSeconds() >= this->timeToMove )
	{	
		// Pick a new direction 
		// Check for collisions, whatever
		
		if ( this->m_pRand )
		{
			// Pick a distance between min and max
			this->target.x = getRandInRange<float>( this->minDistanceToMove, this->maxDistanceToMove, (float)this->m_pRand->getNextRandDouble() );
			//this->target.y = getRandInRange<float>( this->minDistanceToMove, this->maxDistanceToMove, (float)this->m_pRand->getNextRandDouble() );
			this->target.y = 0.0f;
			this->target.z = getRandInRange<float>( this->minDistanceToMove, this->maxDistanceToMove, (float)this->m_pRand->getNextRandDouble() );
			// Randomly "flip" direction of travel (50 percent chance of this)
			if ( this->m_pRand->getNextRandDouble() >= 0.5 ) {	this->target.x = -this->target.x;	}
			if ( this->m_pRand->getNextRandDouble() >= 0.5 ) {	this->target.y = -this->target.y;	}
			if ( this->m_pRand->getNextRandDouble() >= 0.5 ) {	this->target.z = -this->target.z;	}

			// Determine how long this will take
			// Always move at max velocity
			this->timeToMove = fabs( glm::length(this->target) / this->maxVelocity );

			this->velocity = glm::normalize(this->target) * this->maxVelocity;

//			std::cout << "Dalek " << this->m_DalekID_or_Index << " picking new path. ";
//			std::cout << "Time to move = " << this->timeToMove << std::endl;
//			std::cout.flush();

		}//if ( this->m_pRand )

		this->m_pTimer->ResetLongDuration();
		this->m_pTimer->ResetAndStart();
	}
	else
	{

#ifdef DEBUG_PRINT_STATUS_TO_CONSOLE 
	std::cout << "Dalek " << this->m_DalekID_or_Index << " now at: "
		<< this->position.x << ", "
		<< this->position.y << ", "
		<< this->position.z << ") " 
		<< this->timeToMove << " s remaining" << std::endl;
	std::cout.flush();
#endif

		// Move
		float deltaTime = this->m_pTimer->GetElapsedSeconds(true);

		this->position += this->velocity * deltaTime;

		//std::cout << "TimeToMove: " << this->timeToMove << " current: " 
		//	<< this->m_pTimer->get_fLondDurationTotalSeconds() << std::endl;
		//std::cout.flush();

		// Update this in the Dalek Manager

		// This updates the Dalek's location in the central store of positions.
		// (the one that is thread safe)

		if ( this->m_pDalekManager )
		{
				this->m_pDalekManager->setDalekPositionAtIndex( this->m_DalekID_or_Index, this->position );
		}
	}

	// Check for collisions


	return;
}

void cDalek::setDalkeManager(iDalekManager* pDM)
{
	this->m_pDalekManager = pDM;
	return;
}

void cDalek::setRandThreaded(cRandThreaded* pRT)
{
	this->m_pRand = pRT;
	return;
}
