#pragma once
		
#include"myInterface.h"
#include <GLFW/glfw3.h>
class myPhysics : public myInterface
{
public:

	//virtual ~myPhysics();
	virtual void ReadFileToToken(std::ifstream &file, std::string token);
	virtual std::vector<object1> InitializePhysics(std::string file);
	virtual void PhysicsStep(double deltaTime, force &wforce, force &sforce, force &leftforce, force &rightforce, force &upforce, std::vector<glm::vec3> &disp, std::vector<glm::vec3> &rot, std::vector<object1> &my, std::vector<int> &collision);

};



