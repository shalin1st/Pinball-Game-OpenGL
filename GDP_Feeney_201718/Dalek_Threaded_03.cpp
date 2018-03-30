#include "Dalek_Threaded_03.h"

#include "cGameObject.h"
#include "cDalek.h"

#include <iostream>



cDalekManager03::cDalekManager03()
{
// No locks in this version
//	InitializeCriticalSection( &(this->m_cs_DalekDataLock) );

	if ( ! ::g_pThreadedRandom )
	{
		::g_pThreadedRandom = new cRandThreaded();
	}

	// There are 4 buffers, that have a buffer in between them.
	// To start:
	// - buffer 0 : read
	// - buffer 1 : 
	// - buffer 2 : write
	// - buffer 3 :
	this->m_currentReadBufferID = 0;
	this->m_currentWriteBufferID = 2;

	this->m_NumberOfDaleks = 0;

	this->m_nextDalekID = 0;

	return;
}

// Returns the next buffer, as a 'ring', so will:
//	- return currentBuffer + 1 if LT 4
//	- return 0 if GT 3
// (i.e. we go 0, 1, 2, 3, then back to 0 again
unsigned int cDalekManager03::m_calcNextBuffer(unsigned int currentBuffer)
{
	currentBuffer++;

	if ( currentBuffer >= cDalekManager03::NUMBER_OF_BUFFERS )
	{
		// is 'ring buffer', so go back to the 1st index
		currentBuffer = 0;
	}
	return currentBuffer;
}


bool cDalekManager03::CreateDalekThread( cGameObject* pGameObject, cDalek* &pDalek )
{
	if ( this->m_nextDalekID >= this->m_vecDalekPosition[0].size() )
	{	// Too many Daleks
		return false;
	}

	pDalek = new cDalek(this->m_nextDalekID);
	pDalek->setDalkeManager( this );
	pDalek->setRandThreaded( g_pThreadedRandom );

	pDalek->minDistanceToMove = 50.0f;
	pDalek->maxDistanceToMove = 250.0f;
	pDalek->maxVelocity = 25.0f;

	pDalek->position = pGameObject->GetPhysState().position;

	this->m_setDalekPositionAtIndex_ALL_BUFFERS( pDalek->getDalekID(), pDalek->position );

	this->m_vec_pDaleks[pDalek->getDalekID()] = pDalek;


	pDalek->threadInfo.handle = CreateThread( 
	                NULL, // Attributes
		            0,		// 0 = default stack size,
	                DalekBrainThread, 
	                (void*)pDalek,
	                0,			// 0 or CREATE_SUSPENDED, 
	                &(pDalek->threadInfo.address) );

	this->m_nextDalekID++;

	return true;
}



bool cDalekManager03::Init(unsigned int numberOfDaleks)
{
// No locks in this version
//	this->m_LockDalekData();

	this->m_NumberOfDaleks = numberOfDaleks;

	// Load up positions for 500 Daleks... 
	// The values aren't important as they will be overwritten by the Daleks themselves, later
	for ( int count = 0; count != this->m_NumberOfDaleks * 2; count++ )
	{
		for ( int bufferID = 0; bufferID != cDalekManager03::NUMBER_OF_BUFFERS; bufferID++ )
		{
			// Make an "empty" Dalek at this index
			// NOTE: there are 4 identical buffers in this situation
			this->m_vecDalekPosition[bufferID].push_back(glm::vec3(0.0f));
		}
		// Make an "empty" thread info at this index, too
		this->m_vecDalekThreadInfo.push_back( sDalekThreadInfo() );
		//
		this->m_vec_pDaleks.push_back( NULL );
	}
// No locks in this version
//	this->m_UnlockDalekData();

	return true;
}


cDalekManager03::~cDalekManager03()
{
//	DeleteCriticalSection( &(this->m_cs_DalekDataLock) );

	// Kill all the threads...
	for ( unsigned int index = 0; index != this->m_NumberOfDaleks; index++ )
	{
		// Sets to false, forcing thread to exit
		this->m_vec_pDaleks[index]->bIsAlive = false;
	}

	return;
}

//inline 
bool cDalekManager03::m_IsIndexInRange(unsigned int index)
{
	if ( index < this->m_NumberOfDaleks )
	{
		return true;
	}
	return false;
}


// From iDalekManager
bool cDalekManager03::getDalekPositionAtIndex(unsigned int index, glm::vec3 &position)
{
	if ( ! this->m_IsIndexInRange(index) )
	{
		// Index is out of range
		return false;
	}

//	this->m_LockDalekData();
	// Read the position of the dalek from the current read buffer
	position = this->m_vecDalekPosition[this->m_currentReadBufferID][index];
//	this->m_UnlockDalekData();

	return true;
}

// From iDalekManager
// Is only called by the Daleks... 
bool cDalekManager03::setDalekPositionAtIndex(unsigned int index, glm::vec3 position)
{
	if (!this->m_IsIndexInRange(index))
	{
		// Index is out of range
		return false;
	}

//	this->m_LockDalekData();
	this->m_vecDalekPosition[this->m_currentWriteBufferID][index] = position;
//	this->m_UnlockDalekData();

	return true;
}

// Only called when creating a new Dalek... 
// Writes to ALL buffers 
void cDalekManager03::m_setDalekPositionAtIndex_ALL_BUFFERS(unsigned int index, glm::vec3 position)
{
	if ( ! this->m_IsIndexInRange(index) )
	{	// Invalid index. Seriously, what ARE you doing with your life? Get your act together, man!
		return;
	}

	for ( unsigned int bufferID = 0; bufferID != cDalekManager03::NUMBER_OF_BUFFERS; bufferID++ )
	{
		this->m_vecDalekPosition[bufferID][index] = position;
	}
	return;
}


// From iDalekManger
bool cDalekManager03::getAllDalekPositions(std::vector<glm::vec3> &vecDalekPositions)
{
//	this->m_LockDalekData();
	for ( unsigned int index = 0; index != this->m_NumberOfDaleks; index++ )
	{
		//assert(index <= this->m_NumberOfDaleks);
		vecDalekPositions[index] = this->m_vecDalekPosition[this->m_currentReadBufferID][index];
	}
	// Or...
	//std::copy( this->m_vecDalekPosition.begin(), this->m_vecDalekPosition.end(), vecDalekPositions );
//	this->m_UnlockDalekData();

	return true;
}

// Every frame (60 Hz, or 16 ms)
void cDalekManager03::SwitchBuffers(void)
{
	unsigned int nextReadBufferIndex = this->m_calcNextBuffer(this->m_currentReadBufferID);
	unsigned int nextWriteBufferIndex = this->m_calcNextBuffer(this->m_currentWriteBufferID);

	this->m_currentWriteBufferID = nextWriteBufferIndex;
	this->m_currentReadBufferID = nextWriteBufferIndex;

	return;
}

// Sets the bIsAlive to false, exiting the thread
void cDalekManager03::KillAllDaleks(void)
{
	for ( unsigned int index = 0; index != this->m_NumberOfDaleks; index++ )
	{
		this->m_vec_pDaleks[index]->bIsAlive = false;
	}

	return;
}

// Sets bIsUpdating on all Daleks
void cDalekManager03::SetIsUpdatingOnAllDaleks(bool bIsUpdating)
{
	for ( unsigned int index = 0; index != this->m_NumberOfDaleks; index++ )
	{
		this->m_vec_pDaleks[index]->bIsUpdating = bIsUpdating;
	}

	return;
}
