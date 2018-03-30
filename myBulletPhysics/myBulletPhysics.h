#pragma once
#include <btBulletDynamicsCommon.h>
#include<btBulletCollisionCommon.h>
#include"myInterface.h"
#include <globalOpenGL_GLFW.h>
//#include<vector>
class myBulletPhysics : public myInterface
{
public:

	//virtual ~myPhysics();
	virtual void ReadFileToToken(std::ifstream &file, std::string token);
	virtual std::vector<object1> InitializePhysics(std::string file);
	virtual void PhysicsStep(double deltaTime, force &wforce, force &sforce, force &leftforce, force &rightforce, force &upforce, std::vector<glm::vec3> &disp, std::vector<glm::vec3> &rot, std::vector<object1> &my, std::vector<int> &collision);
	
	
	
	
	std::vector <btCollisionShape*> ballshapes;
	std::vector<btRigidBody*> rigidbodies;
	std::vector<btDefaultMotionState*> motionstates;
	std::vector<btScalar> masses;
	std::vector<btVector3> ballInertias;
	btBroadphaseInterface* broadphase = new btDbvtBroadphase();
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
	btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
};



