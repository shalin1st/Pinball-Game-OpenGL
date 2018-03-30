#ifndef _cVertex_xyz_rgb_n_HG_
#define _cVertex_xyz_rgb_n_HG_

#error "Don't include this file! It's been replaced with a newer one that has RGBA and UV coords!!"

class cVertex_xyz_rgb_n
{
public:
	cVertex_xyz_rgb_n();	// Constructor
	~cVertex_xyz_rgb_n();	// Destructor
	float x, y, z;
	float r, g, b;
	float nx, ny, nz;
};

#endif
