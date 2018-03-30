#include "Dalek_Threaded_02.h"

#include "cGameObject.h"
#include "cDalek.h"

//// This is the actual threading function
//DWORD WINAPI DalekBrainThread_01(void* pInitialData)	// CreateThread() format
//{
//	cDalek* pDalek = (cDalek*)(pInitialData);
//
//	while ( pDalek->bIsAlive )
//	{
//		// That's it: calls the Update on the object that we passed in...
//		pDalek->Update();
//
//		Sleep(1);
//	}//while ( pDalek->bIsAlive )
//
//	return 0;
//}


cDalekManager02::cDalekManager02()
{
	InitializeCriticalSection( &(this->m_cs_DalekDataLock) );

	if ( ! ::g_pThreadedRandom )
	{
		::g_pThreadedRandom = new cRandThreaded();
	}

	this->m_nextDalekID = 0;

	return;
}

bool cDalekManager02::CreateDalekThread( cGameObject* pGameObject, cDalek* &pDalek )
{
	if ( this->m_nextDalekID >= this->m_vecDalekPosition.size() )
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

	this->m_vec_pDaleks[pDalek->getDalekID()] = pDalek;


	pDalek->threadInfo.handle = CreateThread(	//_beginthreadex()
	                NULL, // Attributes
		            0,		// 0 = default stack size,
//	                DalekBrainThread_01, 
	                DalekBrainThread, 
	                (void*)pDalek,		// Parameters to pass to the thread function
	                0,			// 0 or CREATE_SUSPENDED, ResumeThread()
	                &(pDalek->threadInfo.address) );		// Or NULL
//	DalekBrainThread_01
	this->m_nextDalekID++;

	return true;
}



bool cDalekManager02::Init(unsigned int numberOfDaleks)
{
	this->m_LockDalekData();

	this->m_NumberOfDaleks = numberOfDaleks;

	// Load up positions for 500 Daleks... 
	// The values aren't important as they will be overwritten by the Daleks themselves, later
	for ( int count = 0; count != this->m_NumberOfDaleks; count++ )
	{
		// Make an "empty" Dalek at this index...
		this->m_vecDalekPosition.push_back(glm::vec3(0.0f));
		// ...and make an "empty" thread info at this index, too
		this->m_vecDalekThreadInfo.push_back( sDalekThreadInfo() );

		this->m_vec_pDaleks.push_back( NULL );
	}
	this->m_UnlockDalekData();

	return true;
}


cDalekManager02::~cDalekManager02()
{
	DeleteCriticalSection( &(this->m_cs_DalekDataLock) );
	return;
}

//inline 
bool cDalekManager02::m_IsIndexInRange(unsigned int index)
{
	if ( index < (unsigned int)this->m_vecDalekPosition.size() )
	{
		return true;
	}
	return false;
}


// From iDalekManager
bool cDalekManager02::getDalekPositionAtIndex(unsigned int index, glm::vec3 &position)
{
	if ( ! this->m_IsIndexInRange(index) )
	{
		// Index is out of range
		return false;
	}

	this->m_LockDalekData();
	position = this->m_vecDalekPosition[index];
	this->m_UnlockDalekData();

	return true;
}

// From iDalekManager
// Is only called by the Daleks... 
bool cDalekManager02::setDalekPositionAtIndex(unsigned int index, glm::vec3 position)
{
	if (!this->m_IsIndexInRange(index))
	{
		// Index is out of range
		return false;
	}

	this->m_LockDalekData();		// Enter Critical Section
	this->m_vecDalekPosition[index] = position;
	this->m_UnlockDalekData();		// Leave Critical Section

	return true;
}

// From iDalekManger
bool cDalekManager02::getAllDalekPositions(std::vector<glm::vec3> &vecDalekPositions)
{
	this->m_LockDalekData();		// Enter Critical Section
	for ( unsigned int index = 0; index != this->m_NumberOfDaleks; index++ )
	{
		vecDalekPositions[index] = this->m_vecDalekPosition[index];
	}
	// Or...
	//std::copy( this->m_vecDalekPosition.begin(), this->m_vecDalekPosition.end(), vecDalekPositions );
	this->m_UnlockDalekData();		// Leave Critical Section

	return true;
}

// From iDalekManger
bool cDalekManager02::cDalekManager02::IsDataLocked(void)
{
	return this->m_bIsLocked;
}

// ******************************************

void cDalekManager02::m_LockDalekData(void)
{
	EnterCriticalSection( &(this->m_cs_DalekDataLock) );
	this->m_bIsLocked = true;
	return;
}

void cDalekManager02::m_UnlockDalekData(void)
{
	this->m_bIsLocked = false;
	LeaveCriticalSection( &(this->m_cs_DalekDataLock) );
	return;
}


// Sets the bIsAlive to false, exiting the thread
void cDalekManager02::KillAllDaleks(void)
{
	this->m_LockDalekData();
	for ( unsigned int index = 0; index != this->m_NumberOfDaleks; index++ )
	{
		this->m_vec_pDaleks[index]->bIsAlive = false;
	}
	this->m_UnlockDalekData();

	return;
}

// Sets bIsUpdating on all Daleks
void cDalekManager02::SetIsUpdatingOnAllDaleks(bool bIsUpdating)
{
	this->m_LockDalekData();
	for ( unsigned int index = 0; index != this->m_NumberOfDaleks; index++ )
	{
		this->m_vec_pDaleks[index]->bIsUpdating = bIsUpdating;
	}
	this->m_UnlockDalekData();

	return;
}
