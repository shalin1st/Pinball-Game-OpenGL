#include"myBulletPhysics.h"



void myBulletPhysics::ReadFileToToken(std::ifstream &file, std::string token)
{
	bool bKeepReading = true;
	std::string garbage;
	do
	{
		file >> garbage;		// Title_End??
		if (garbage == token)
		{
			return;
		}
	} while (bKeepReading);
	return;
}



std::vector<myBulletPhysics::object1> myBulletPhysics::InitializePhysics(std::string file)
{

	//Physics Bullet Initializer

	
	dynamicsWorld->setGravity(btVector3(0, -10, 0));


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//GROUND SHAPE DEFINITION
	btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -2, 0)));
	btRigidBody::btRigidBodyConstructionInfo  groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
	btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
	dynamicsWorld->addRigidBody(groundRigidBody);


	//BACKWALL SHAPE DEFINITION
	btCollisionShape* BackWallShape = new btStaticPlaneShape(btVector3(0, 0, 1), 0);
	btDefaultMotionState* BackWallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, -14)));
	btRigidBody::btRigidBodyConstructionInfo  BackWallRigidBodyCI(0, BackWallMotionState, BackWallShape, btVector3(0, 0, 0));
	btRigidBody* BackWallRigidBody = new btRigidBody(BackWallRigidBodyCI);
	dynamicsWorld->addRigidBody(BackWallRigidBody);

	//LeftWall SHAPE DEFINITION
	btCollisionShape* LeftWallShape = new btStaticPlaneShape(btVector3(1, 0, 0), 0);
	btDefaultMotionState* LeftWallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(-14, 0, 0)));
	btRigidBody::btRigidBodyConstructionInfo  LeftWallRigidBodyCI(0, LeftWallMotionState, LeftWallShape, btVector3(0, 0, 0));
	btRigidBody* LeftWallRigidBody = new btRigidBody(LeftWallRigidBodyCI);
	dynamicsWorld->addRigidBody(LeftWallRigidBody);

	//RightWall SHAPE DEFINITION
	btCollisionShape* RightWallShape = new btStaticPlaneShape(btVector3(-1, 0, 0), 0);
	btDefaultMotionState* RightWallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(16, 0, 0)));
	btRigidBody::btRigidBodyConstructionInfo  RightWallRigidBodyCI(0, RightWallMotionState, RightWallShape, btVector3(0, 0, 0));
	btRigidBody* RightWallRigidBody = new btRigidBody(RightWallRigidBodyCI);
	dynamicsWorld->addRigidBody(RightWallRigidBody);

	//FRONTWALL SHAPE DEFINITION
	btCollisionShape* FrontWallShape = new btStaticPlaneShape(btVector3(0, 0, -1), 0);
	btDefaultMotionState* FrontWallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 16)));
	btRigidBody::btRigidBodyConstructionInfo  FrontWallRigidBodyCI(0, FrontWallMotionState, FrontWallShape, btVector3(0, 0, 0));
	btRigidBody* FrontWallRigidBody = new btRigidBody(FrontWallRigidBodyCI);
	dynamicsWorld->addRigidBody(FrontWallRigidBody);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	std::vector <object1> myvec;
	object1 myobj;
	std::ifstream plyFile(file.c_str());

	if (!plyFile.is_open())
	{	// Didn't open file, so return

	}


	ReadFileToToken(plyFile, "models");
	int count;
	plyFile >> count;

	for (int i = 1; i <= count; i++)
	{

		ReadFileToToken(plyFile, std::to_string(i));
		ReadFileToToken(plyFile, "position");
		plyFile >> myobj.pos.x;
		plyFile >> myobj.pos.y;
		plyFile >> myobj.pos.z;

		ReadFileToToken(plyFile, "orientation");
		plyFile >> myobj.ori.x;
		plyFile >> myobj.ori.y;
		plyFile >> myobj.ori.z;


		ReadFileToToken(plyFile, "orientation2");
		plyFile >> myobj.or2.x;
		plyFile >> myobj.or2.y;
		plyFile >> myobj.or2.z;

		ReadFileToToken(plyFile, "scale");
		plyFile >> myobj.scale;

		ReadFileToToken(plyFile, "color");
		plyFile >> myobj.color.a;
		plyFile >> myobj.color.r;
		plyFile >> myobj.color.g;
		plyFile >> myobj.color.b;

		ReadFileToToken(plyFile, "name");
		plyFile >> myobj.name;

		ReadFileToToken(plyFile, "type");
		plyFile >> myobj.typer;
		if (myobj.typer == "sphere")
		{
			myobj.type = eTypeOfObject::SPHERE;
		}
		else
		{
			myobj.type = eTypeOfObject::PLANE;
		}

		ReadFileToToken(plyFile, "physics");
		plyFile >> myobj.phys;


		if (myobj.typer == "plane")
		{
			masses.push_back(0);
			ballshapes.push_back(new btStaticPlaneShape(btVector3(0, 2, 0), 1));
			
		}
		else
		{
			masses.push_back(myobj.scale);
			ballshapes.push_back(new btSphereShape(myobj.scale));
		}
		
		motionstates.push_back(new btDefaultMotionState((btTransform(btQuaternion(0, 0, 0, 1), btVector3(myobj.pos.x, myobj.pos.y, myobj.pos.z)))));
		ballInertias.push_back(btVector3(0,0,0));
		
		myvec.push_back(myobj);

	}



	for (int i = 0; i < myvec.size(); i++)
	{

		ballshapes[i]->calculateLocalInertia(masses[i], ballInertias[i]);
		rigidbodies.push_back(new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(masses[i], motionstates[i], ballshapes[i], ballInertias[i])));
	}

	for (int i = 1; i < rigidbodies.size()-4; i++)
	{
		dynamicsWorld->addRigidBody(rigidbodies[i]);

	}
	return myvec;
}






void myBulletPhysics::PhysicsStep(double deltaTime, force &wforce, force &sforce, force &leftforce, force &rightforce, force &upforce, std::vector<glm::vec3> &disp, std::vector<glm::vec3> &rot, std::vector<object1> &my, std::vector<int> &collision)
{

	dynamicsWorld->stepSimulation((float)deltaTime, 100);

	btTransform trans;
	btQuaternion orient;
	btTransform trans2;
	float xr, yr, zy;
	for (int i = 0; i < rigidbodies.size(); i++)
	{
		
		rigidbodies[i]->getMotionState()->getWorldTransform(trans);
		orient = trans.getRotation();
		

		disp[i] = glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ()) - my[i].pos;
		my[i].pos = glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());

		if (my[i].typer != "plane")
		{
			//rot[i] = ( (glm::vec3(orient.x(), orient.y(), orient.z()) - my[i].or2) / 5.0f ) ;
			//my[i].or2 = glm::vec3(orient.x(), orient.y(), orient.z());
			rot[i] = ((glm::cross(glm::vec3(0, 1, 0), disp[i])))/2.0f;
		}
		else
		{
			
		}
		if (wforce.on == 1)
		{
			
			rigidbodies[1]->applyCentralImpulse( btVector3(wforce.direction.x, wforce.direction.y, wforce.direction.z)/700);
		}
		if (rightforce.on == 1)
		{

			rigidbodies[1]->applyCentralImpulse(btVector3(rightforce.direction.x, rightforce.direction.y, rightforce.direction.z) / 700);
		}
		if (leftforce.on == 1)
		{

			rigidbodies[1]->applyCentralImpulse(btVector3(leftforce.direction.x, leftforce.direction.y, leftforce.direction.z) / 700);
		}
		if (sforce.on == 1)
		{

			rigidbodies[1]->applyCentralImpulse(btVector3(sforce.direction.x, sforce.direction.y, sforce.direction.z) / 700);
		}
	


		std::cout << my[i].pos.y<<std::endl;

	}

	int i = 1;
	
	
	//Sleep(50);





	return;
}



