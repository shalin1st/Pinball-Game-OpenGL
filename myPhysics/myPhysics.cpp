#include"myPhysics.h"



void myPhysics::ReadFileToToken(std::ifstream &file, std::string token)
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



std::vector<myPhysics::object1> myPhysics::InitializePhysics(std::string file)
{

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

		myvec.push_back(myobj);

	}
	return myvec;
}






void myPhysics::PhysicsStep(double deltaTime, force &wforce, force &sforce, force &leftforce, force &rightforce, force &upforce, std::vector<glm::vec3> &disp, std::vector<glm::vec3> &rot, std::vector<object1> &my, std::vector<int> &collision)
{


	if (collision.size() != my.size())
	{
		for (int i = 0; i < my.size(); i++)
		{
			collision.push_back(0);
		}
	}
	
	const glm::vec3 GRAVITY = glm::vec3(0.0f, -5.0f, 0.0f);
	for (int index = 0; index < my.size(); index++)
	{

		// Is this object to be updated?
		object1 pCurGo = my[index];
		if (!pCurGo.phys)
		{
			continue;
		}

		glm::vec3 a;
		a = (GRAVITY);
		glm::vec3 deltaVelocity = ((a*(float)deltaTime / 4.0f) + ((a*(float)deltaTime / 2.0f)) + (3.0f*(a*(float)deltaTime / 2.0f)) + (a*(float)deltaTime)) / 4.0f;
		glm::vec3 deltaVelocity2 = ((float)deltaTime * a);
		switch (pCurGo.type)
		{
		case eTypeOfObject::SPHERE:

			const float SURFACEOFGROUND = -1.5f;
			const float LEFTSIDEWALL = 14.5f;
			const float RIGHTSIDEWALL = -13.5f;

			const float BACKWALL = -13.5f;
			const float FRONTWALL = 14.5f;
			// 

			if (index == 1)
			{
				if (wforce.on == 1)
				{
					a = (wforce.direction * wforce.strength * glm::vec3(1, 0, 1)) * 15.0f;
					deltaVelocity = ((a*(float)deltaTime / 4.0f) + ((a*(float)deltaTime / 2.0f)) + (3.0f*(a*(float)deltaTime / 4.0f)) + (a*(float)deltaTime)) / 6.0f;

					pCurGo.vel += deltaVelocity;

				}
				else
				{
					if (abs(pCurGo.vel.x) > 0 || abs(pCurGo.vel.z) >0)
						pCurGo.vel -= pCurGo.vel  * glm::vec3(1, 0, 1) / 20.0f;
				}

				if (sforce.on == 1)
				{
					a = (-wforce.direction * wforce.strength * glm::vec3(1, 0, 1)) * 15.0f;
					deltaVelocity = ((a*(float)deltaTime / 4.0f) + ((a*(float)deltaTime / 2.0f)) + (3.0f*(a*(float)deltaTime / 4.0f)) + (a*(float)deltaTime)) / 6.0f;

					pCurGo.vel += deltaVelocity;

				}
				else
				{
					if (abs(pCurGo.vel.x) > 0 || abs(pCurGo.vel.z) >0)
						pCurGo.vel -= pCurGo.vel  * glm::vec3(1, 0, 1) / 20.0f;
				}

				if (leftforce.on == 1)
				{
					a = (leftforce.direction * leftforce.strength * glm::vec3(1, 0, 1)) * 15.0f;
					deltaVelocity = ((a*(float)deltaTime / 4.0f) + ((a*(float)deltaTime / 2.0f)) + (3.0f*(a*(float)deltaTime / 4.0f)) + (a*(float)deltaTime)) / 4.0f;

					pCurGo.vel += deltaVelocity;

				}

				else
				{
					if (abs(pCurGo.vel.x) > 0 || abs(pCurGo.vel.z) >0)
						pCurGo.vel -= pCurGo.vel  * glm::vec3(1, 0, 1) / 20.0f;
				}

				if (rightforce.on == 1)
				{
					a = (rightforce.direction * rightforce.strength * glm::vec3(1, 0, 1)) * 15.0f;
					deltaVelocity = ((a*(float)deltaTime / 4.0f) + ((a*(float)deltaTime / 2.0f)) + (3.0f*(a*(float)deltaTime / 4.0f)) + (a*(float)deltaTime)) / 4.0f;

					pCurGo.vel += deltaVelocity;

				}
				else
				{
					if (abs(pCurGo.vel.x) > 0 || abs(pCurGo.vel.z) >0)
						pCurGo.vel -= pCurGo.vel  * glm::vec3(1, 0, 1) / 20.0f;
				}

				if (upforce.on == 1)
				{
					deltaVelocity += upforce.direction * upforce.strength;
					pCurGo.vel += deltaVelocity;
				}
				else
				{
					if ((pCurGo.vel.y) > 0)
						pCurGo.vel.y -= pCurGo.vel.y / 2.0f;
				}




			}
		




			for (int j = 0; j < my.size(); j++)
			{
				if (my[j].type == eTypeOfObject::SPHERE && index != j)
				{


					if (glm::distance(my[index].pos, my[j].pos) <= my[index].scale + my[j].scale)
					{


						my[j].vel += (my[j].pos - my[index].pos)*(float)deltaTime / 2.0f + my[index].vel*(float)deltaTime;
						collision[index] = 1;
						collision[j] = 1;
					}
					else
					{
						if (j != 1)
						{
							if (abs(my[j].vel.x) > 0 || abs(my[j].vel.z) > 0)
							{
								my[j].vel -= my[j].vel  * glm::vec3(1, 0, 1) * my[j].scale / 600.0f;
								collision[j] = 2;
							}
						}
						
					}
				}
			}
			


			if ((pCurGo.pos.y - pCurGo.scale / 2.0f) <= SURFACEOFGROUND && upforce.on == 0)
			{
				pCurGo.vel.y = 0;
				deltaVelocity.y = 0;
			}

			if ((pCurGo.pos.x - pCurGo.scale / 2.0f) <= RIGHTSIDEWALL && (float)deltaTime*pCurGo.vel.x<0)
			{
				pCurGo.vel.x = 0;
				deltaVelocity.x = 0;
			}

			if ((pCurGo.pos.x - pCurGo.scale / 2.0f) >= LEFTSIDEWALL && (float)deltaTime*pCurGo.vel.x>0)
			{
				pCurGo.vel.x = 0;
				deltaVelocity.x = 0;
	
			}

			if ((pCurGo.pos.z - pCurGo.scale / 2.0f) <= BACKWALL && (float)deltaTime*pCurGo.vel.z<0)
			{
				pCurGo.vel.z = 0;
				deltaVelocity.z = 0;
			}

			if ((pCurGo.pos.z - pCurGo.scale / 2.0f) >= FRONTWALL && (float)deltaTime*pCurGo.vel.z>0)
			{
				pCurGo.vel.z = 0;
				deltaVelocity.z = 0;
			}



			pCurGo.vel = pCurGo.vel + deltaVelocity;

			glm::vec3 deltaPosition = (float)deltaTime * pCurGo.vel;



			rot[index] = (glm::cross(glm::vec3(0, 1, 0), deltaPosition) / 2.0f);
			disp[index] = (deltaPosition);
			my[index].vel = pCurGo.vel;
			my[index].pos += deltaPosition;
			my[index].or2 += ((glm::cross(glm::vec3(0, 1, 0), deltaPosition) / 2.0f));

			if (deltaPosition.x <= 0.001f && deltaPosition.z <= 0.001f)
			{
				collision[index] = 0;
			}
			break;
		};

	}

	return;
}



