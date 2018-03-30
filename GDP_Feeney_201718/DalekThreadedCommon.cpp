// DalekThreadedCommon

#include "cGameObject.h"
#include "DalekThreadedCommon.h"

#include "cDalek.h"

cRandThreaded* g_pThreadedRandom = NULL;


// NOTE: Should be aligned on 32 bit boundtry (see _aligned_malloc)
extern int unsigned* g_pCS_per_fram_count = NULL;


cGameObject* MakeDalekGameObject(glm::vec3 initPosition)
{
	cGameObject* pDalek = new cGameObject();
	pDalek->friendlyName = "Big D";
	cPhysicalProperties physState;
	physState.position = initPosition;
	physState.setOrientationEulerAngles(glm::vec3(0.0, 0.0, 0.0f));
	pDalek->SetPhysState(physState);
	sMeshDrawInfo meshInfo;
	meshInfo.scale = 1.0;
	meshInfo.name = "dalek2005_xyz_uv_res_2.ply";
	meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("GuysOnSharkUnicorn.bmp", 0.0f));
	meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("Utah_Teapot_xyz_n_uv_Enterprise.bmp", 1.0f));
	meshInfo.setMeshOrientationEulerAngles(glm::vec3(-90.0f, 0.0f, 0.0f), true);
	pDalek->vecMeshes.push_back(meshInfo);

	return pDalek;
}

// This is the actual threading function
DWORD WINAPI DalekBrainThread(void* pInitialData)	// CreateThread() format
{
	cDalek* pDalek = (cDalek*)(pInitialData);

	while ( pDalek->bIsAlive )	// 0 or non-zero
	{
		if ( pDalek->bIsUpdating )
		{
			// That's it: calls the Update on the object that we passed in...

			pDalek->Update();

			Sleep(1);		// 16 ms
		}
		else
		{	
			// Sleep for a while...
			// This is likely happening during the start up phase
			Sleep(500);
		}
	}//while ( pDalek->bIsAlive )

	return 0;
}
