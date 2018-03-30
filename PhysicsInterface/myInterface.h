#pragma once
//#include"structs.h"
#include <glad/glad.h>
#include <globalOpenGL_GLFW.h>
#include <iostream>			
#include <glm/vec3.hpp> 
#include <glm/vec4.hpp> 
#include <glm/mat4x4.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp> 
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <sstream>		
#include <string>
#include <vector>
#include <glm/vec3.hpp> 
#include <glm/vec4.hpp> 
#include<iostream>
#include<vector>

class myInterface
{
public: 
	
	virtual ~myInterface() {}
	struct force
	{
		int on;
		float strength;
		glm::vec3 direction;

	};
	enum eTypeOfObject
	{	// Ok people, here's the deal:
		SPHERE = 0,		// - it's a sphere
		PLANE = 1,		// - it's a plane
		CAPSULE = 2,    // - it's a capsule
		AABB = 3,		// 3- it's an axis-aligned box
		UNKNOWN = 99	// I don't know
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

	
	virtual void ReadFileToToken(std::ifstream &file, std::string token) = 0;
	virtual std::vector<object1> InitializePhysics(std::string file) = 0;
	virtual void PhysicsStep(double deltaTime, force &wforce, force &sforce, force &leftforce, force &rightforce, force &upforce, std::vector<glm::vec3> &disp, std::vector<glm::vec3> &rot, std::vector<object1> &my, std::vector<int> &collision) = 0;
};