#ifndef _DALEK_THREADED_02_HG_
#define _DALEK_THREADED_02_HG_

#include "iDalekManager.h"
#include <process.h>
#include <Windows.h>		// For CRITICAL_SECTION, CreateThread, ResumeThread, etc.
#include "Utilities.h"


// Dalek threading solution-example #1
// 
// Locked version, using a critical section lock.
// 
// - The shared data (dalek position) is placed in central location (in manager class)
//   (this is thhe "std::vector<glm::vec3> m_vecDalekPosition" vector
// - It is protected by a crtical section lock
// - Dalaks write to a single location (index)
// - All Dalek positions are read by both the Daleks and the game loop
//   - This read is also protected by THE SAME lock


#include "DalekThreadedCommon.h"



class cDalekManager02 : public iDalekManager
{
public:
	cDalekManager02();
	virtual ~cDalekManager02();

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

	// Sets the bIsAlive to false, exiting the thread
	virtual void KillAllDaleks(void);

	// Sets bIsUpdating on all Daleks
	virtual void SetIsUpdatingOnAllDaleks(bool bIsUpdating);

	virtual bool IsDataLocked(void);

	// Used in the lock-less or lock-free variant 
	// Does nothing in this variant
	virtual void SwitchBuffers(void)
	{
		return;
	}


private:
	void m_LockDalekData(void);
	void m_UnlockDalekData(void);

	unsigned int m_NumberOfDaleks;// = 500;

	bool m_bIsLocked;

	CRITICAL_SECTION m_cs_DalekDataLock;

	std::vector<glm::vec3> m_vecDalekPosition;
	// In order to kill or make active
	std::vector<cDalek*> m_vec_pDaleks;

	// Saves the thread handle, etc.
	std::vector<sDalekThreadInfo> m_vecDalekThreadInfo;

	inline bool m_IsIndexInRange(unsigned int index);

	// When creating a Dalek, this is the next index in the array (vector)
	unsigned int m_nextDalekID;// = 0;
};

#endif
