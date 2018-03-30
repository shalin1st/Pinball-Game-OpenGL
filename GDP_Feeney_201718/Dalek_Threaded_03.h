#ifndef _DALEK_THREADED_03_HG_
#define _DALEK_THREADED_03_HG_

#include "iDalekManager.h"
#include <process.h>
#include <Windows.h>		// For CRITICAL_SECTION, CreateThread, ResumeThread, etc.
#include "Utilities.h"


// Dalek threading solution-example #2
// 
// Lock-less or lock-free version.


#include "DalekThreadedCommon.h"


class cDalekManager03 : public iDalekManager
{
public:
	cDalekManager03();
	virtual ~cDalekManager03();

	virtual bool Init(unsigned int numberOfDaleks);

	// Spawns the next Dalek, using the game object position
	// returns false if there aren't enough 'slots' for the Daleks
	virtual bool CreateDalekThread( cGameObject* pGameObject, cDalek* &pDalek );

	// All of these call lock and unlock
	virtual bool getDalekPositionAtIndex(unsigned int index, glm::vec3 &position);

	// Called by each Dalek...
	virtual bool setDalekPositionAtIndex(unsigned int index, glm::vec3 position);
	// Called by Daleks and the main render loop
	// NOTE: Passed vector MUST be allocated BEFORE call
	virtual bool getAllDalekPositions( std::vector<glm::vec3> &vecDalekPositions );

	// Does nothing in this version as there are no locks...
	virtual bool IsDataLocked(void)
	{
		// no locks, so always returns "unlocked", yo!
		return false;
	}

	// Used in the lock-less or lock-free variant 
	virtual void SwitchBuffers(void);

	// Sets the bIsAlive to false, exiting the thread
	virtual void KillAllDaleks(void);

	// Sets bIsUpdating on all Daleks
	virtual void SetIsUpdatingOnAllDaleks(bool bIsUpdating);



private:
//	void m_LockDalekData(void);
//	void m_UnlockDalekData(void);

	unsigned int m_NumberOfDaleks;// = 500;

// No critical section as there aren't any locks...
//	bool m_bIsLocked;
//	CRITICAL_SECTION m_cs_DalekDataLock;

	static const unsigned int NUMBER_OF_BUFFERS = 4;  // 12 x 2 : 11 frames behind
	std::vector<glm::vec3> m_vecDalekPosition[NUMBER_OF_BUFFERS];
	unsigned int m_currentReadBufferID;
	unsigned int m_currentWriteBufferID;
	// Returns the next buffer, as a 'ring', so will:
	//	- return currentBuffer + 1 if LT 4
	//	- return 0 if GT 3
	// (i.e. we go 0, 1, 2, 3, then back to 0 again
	unsigned int m_calcNextBuffer(unsigned int currentBuffer);

	// Only called when creating a new Dalek... 
	// Writes to ALL buffers 
	void m_setDalekPositionAtIndex_ALL_BUFFERS(unsigned int index, glm::vec3 position);


	// Saves the thread handle, etc.
	std::vector<sDalekThreadInfo> m_vecDalekThreadInfo;
	std::vector<cDalek*> m_vec_pDaleks;

	inline bool m_IsIndexInRange(unsigned int index);

	// When creating a Dalek, this is the next index in the array (vector)
	unsigned int m_nextDalekID;// = 0;
};

#endif
