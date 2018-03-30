#pragma once
#include <glm/vec3.hpp> 
#include <glm/vec4.hpp> 
#include<iostream>
#include<vector>
struct force
{
	int on;
	float strength;
	glm::vec3 direction;

};

struct object1
{
	std::string typer;
	glm::vec3 pos; 
	glm::vec3 ori; 
	glm::vec3 or2; 
	float scale;  
	glm::vec4 color; 
	std::string name; 
	eTypeOfObject type; 
	bool phys;
	glm::vec3 vel = glm::vec3(0.0f, 0.0f, 0.0f);
};

enum eTypeOfObject
{	// Ok people, here's the deal:
	SPHERE = 0,		// - it's a sphere
	PLANE = 1,		// - it's a plane
	CAPSULE = 2,    // - it's a capsule
	AABB = 3,		// 3- it's an axis-aligned box
	UNKNOWN = 99	// I don't know
};
