#include "AppClass.h"
using namespace Simplex;
void Application::InitVariables(void)
{
	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUpward(
		vector3(0.0f, 25.0f, 50.0f), //Position
		vector3(0.0f, 0.0f, 0.0f),	//Target
		AXIS_Y);					//Up

	m_pLightMngr->SetPosition(vector3(0.0f, 3.0f, 13.0f), 1); //set the position of first light (0 is reserved for ambient light)

	m_pEntityMngr->AddEntity("Minecraft\\Steve.obj", "Steve");
	m_pEntityMngr->UsePhysicsSolver();
	
	//create location
	locale = {
		//top square
		vector3(-size,size,size),
		vector3(-size,-size,size),
		vector3(size,-size,size),
		vector3(size,size,size),
		//back square
		vector3(-size,size,-size),
		vector3(-size,-size,-size),
		vector3(size,-size,-size),
		vector3(size,size,-size),
	};

	//instanciate oct tree
	tree = new OctLeaf(locale, cubeTemp, 3);

	for (int i = 0; i < 100; i++)
	{
		m_pEntityMngr->AddEntity("Minecraft\\finalSphere.obj", "particle_" + std::to_string(i));
		vector3 v3Position = vector3(glm::sphericalRand(50.0f));
		v3Position.y = 0.0f;
		matrix4 m4Position = glm::translate(v3Position);
		m_pEntityMngr->SetModelMatrix(m4Position * glm::scale(vector3(-2.0f)));
		m_pEntityMngr->UsePhysicsSolver();
		cubeTemp.push_back(m_pEntityMngr->GetRigidBody()); //get the rigid body for the list
		//m_pEntityMngr->SetMass(2);
		//m_pEntityMngr->SetMass(i+1);
	}

}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the ArcBall active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();

	//Update Entity Manager
	m_pEntityMngr->Update();

	//Set the model matrix for the main object
	//m_pEntityMngr->SetModelMatrix(m_m4Steve, "Steve");
	vector3 vel;
	float x;
	float y;
	float z;
	for (int i = 0; i < 100; i++)
	{		
		//randomly generate a vector for velocity
		x = (rand() % 20) / 10;
		y = (rand() % 20) / 10;
		z = (rand() % 20) / 10;
		vel = vector3(x, y, z);
		m_pEntityMngr->SetVelocity(vel, "particle_" + std::to_string(i));
	}
	for (int i = 0; i < 100; i++)
	{
		//check for various collisions/locations
		//if the sphere hits the location on or outside the bounding box, bounce off(i.e go in other direction)
		for (int j = 0; j < locale.size(); j++) //to check if they had crossed the bounding box (checking x y and z of each separately because I con't think of another way to)
		{
			if (m_pEntityMngr->GetPosition("particle_" + std::to_string(i)).x >= locale[j].x)
			{
				if (m_pEntityMngr->GetPosition("particle_" + std::to_string(i)).y >= locale[j].y)
				{
					if (m_pEntityMngr->GetPosition("particle_" + std::to_string(i)).z >= locale[j].z)
					{
						vel = vector3(-x,-y,-z);
						m_pEntityMngr->SetVelocity(vel, "particle_" + std::to_string(i));
					}

				}
			}
			else
			{
				vector3 vel = vector3(x, y, z);
			}
		}
	}
	//Add objects to render list
	m_pEntityMngr->AddEntityToRenderList(-1, true);
	//m_pEntityMngr->AddEntityToRenderList(-1, true);

}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	if (m_uOctantLevels > 0)
	{
		tree->setVisible();
		tree->Subdivide(1, locale);
	}

	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();

	m_pMeshMngr->GenerateWireCube(size);

	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();

	//draw gui,
	DrawGUI();

	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	//Release MyEntityManager
	MyEntityManager::ReleaseInstance();

	//release GUI
	ShutdownGUI();
}