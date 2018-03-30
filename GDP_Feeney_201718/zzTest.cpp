#include "sVertex_xyz_rgba_n_uv2_bt_skinnedMesh.h"
#include <iostream>

int main()
{
	
	unsigned int off 
		= offsetof(sVertex_xyz_rgba_n_uv2_bt_skinnedMesh, boneID[0]);
		
	std::cout << "boneID[0] = " << off << std::endl;
	
	off = offsetof(sVertex_xyz_rgba_n_uv2_bt_skinnedMesh, boneWeights[0]);
	
	std::cout << "boneWeights[0] = " << off << std::endl;
	
	return -1;
}

