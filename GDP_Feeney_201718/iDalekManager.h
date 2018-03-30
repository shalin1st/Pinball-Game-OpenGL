#ifndef _iDALEK_MANAGER_HG_
#define _iDALEK_MANAGER_HG_

#include <glm/glm.hpp>
#include <vector> 

class cGameObject;	// Forward declare to omit include
class cDalek;

class iDalekManager
{
public:
	virtual ~iDalekManager() {};

	// Sets up the protected data in memory (the "thread safe" area)
	virtual bool Init( unsigned int numberOfDaleks ) = 0;

	// Actually creates the thread and starts the process. 
	// Passes the DalekManager and the thread safe rand
	virtual bool CreateDalekThread( cGameObject* pGameObject, cDalek* &pDalek ) = 0;

	// Gets a single Dalek. Not actually called by anything
	virtual bool getDalekPositionAtIndex(unsigned int index, glm::vec3 &position) = 0;
	// Sets the position of a single Dalek. 
	// Called by each Dalek
	virtual bool setDalekPositionAtIndex(unsigned int index, glm::vec3 position) = 0;
	// Called by both Daleks and the game loop
	virtual bool getAllDalekPositions(std::vector<glm::vec3> &vecDalekPositions) = 0;

	// Helper function to check for locking. 
	// Isn't used here, but can be used decide if we should attempt data access
	virtual bool IsDataLocked(void) = 0;

	// Used in the lock-less or lock-free variant 
	// Called each "frame" update
	virtual void SwitchBuffers(void) = 0;

	// Sets the bIsAlive to false, exiting the thread
	virtual void KillAllDaleks(void) = 0;

	// Sets bIsUpdating on all Daleks
	virtual void SetIsUpdatingOnAllDaleks(bool bIsUpdating) = 0;

};

#endif

