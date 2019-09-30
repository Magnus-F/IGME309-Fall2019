#include "AppClass.h"
void Application::InitVariables(void)
{
	//Change this to your name and email
	m_sProgrammer = "Myles Weider - mjw7119@rit.edu";
	
	//Set the position and target of the camera
	//(I'm at [0,0,10], looking at [0,0,0] and up is the positive Y axis)
	m_pCameraMngr->SetPositionTargetAndUpward(AXIS_Z * 20.0f, ZERO_V3, AXIS_Y);

	//if the light position is zero move it
	if (m_pLightMngr->GetPosition(1) == ZERO_V3)
		m_pLightMngr->SetPosition(vector3(0.0f, 0.0f, 3.0f));

	

	//if the background is cornflowerblue change it to black (its easier to see)
	if (vector3(m_v4ClearColor) == C_BLUE_CORNFLOWER)
	{
		m_v4ClearColor = vector4(ZERO_V3, 1.0f);
	}
	
	//if there are no segments create 7
	if(m_uOrbits < 1)
		m_uOrbits = 7;

	float fSize = 1.0f; //initial size of orbits

	//creating a color using the spectrum 
	uint uColor = 650; //650 is Red
	//prevent division by 0
	float decrements = 250.0f / (m_uOrbits > 1? static_cast<float>(m_uOrbits - 1) : 1.0f); //decrement until you get to 400 (which is violet)
	/*
		This part will create the orbits, it start at 3 because that is the minimum subdivisions a torus can have
	*/

	uint uSides = 3; //start with the minimal 3 sides
	for (uint i = uSides; i < m_uOrbits + uSides; i++)
	{
		vector3 v3Color = WaveLengthToRGB(uColor); //calculate color based on wavelength
		m_shapeList.push_back(m_pMeshMngr->GenerateTorus(fSize, fSize - 0.1f, 3, i, v3Color)); //generate a custom torus and add it to the meshmanager




		std::vector<vector3> circle;
		//start with the base point (vector made with center (0,0,0) and fRadius)
		vector3 base = vector3(fSize, 0, 0);
		vector3 next; //would help during for loop later
		//making everything a float below because otherwise it will round to the nearest integer and cause peices in the generated image to be missing
		float degreeInterval = 360.0f / (float)i; //this gets angle that all triangles will have at center
		vector3 center = vector3(0, 0, 0); //not necessary but good for visualization
		//from that we need to account for the other vectors using the angle of the pretend triangle (made with nSubdivisions/360)
		//for every new point it would be basically base * (tangent rule (?) with angle * i)
		for (int j = 0; j < i + 1; j++)
		{
			//calculate necessary numbers
			next = base;
			float radianCalc = (degreeInterval * 3.14159265359 * j) / 180.0f;
			float x = cos(radianCalc) * fSize;
			float y = sin(radianCalc) * fSize;
			//flop em all into next
			next = vector3(x, y, 0);
			//draw the triangle
			circle.push_back(next);
			//forget previous base
			base = next;
		}
		pointStorage.push_back(circle);
		stopStorage.push_back(0);

		fSize += 0.5f; //increment the size for the next orbit
		uColor -= static_cast<uint>(decrements); //decrease the wavelength
	}
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	matrix4 m4View = m_pCameraMngr->GetViewMatrix(); //view Matrix
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix(); //Projection Matrix
	matrix4 m4Offset = IDENTITY_M4; //offset of the orbits, starts as the global coordinate system
	/*
		The following offset will orient the orbits as in the demo, start without it to make your life easier.
	*/
	//m4Offset = glm::rotate(IDENTITY_M4, 1.5708f, AXIS_Z);

	//Get a timer
	static float fTimer = 0;	//store the new timer
	static uint uClock = m_pSystem->GenClock(); //generate a new clock for that timer
	fTimer += m_pSystem->GetDeltaTime(uClock); //get the delta time for that timer

	//get base stuff
	vector3 start;
	vector3 stop;
	float timeBetween = 1.0f;
	float percentageOfTime = MapValue(fTimer, 0.0f, timeBetween, 0.0f, 1.0f);


	// draw a shapes
	for (uint i = 0; i < m_uOrbits; ++i)
	{
		//possible issues
		//	not exiting for loop (becoming trapped in for loop) (could also be source for crash error)
		//	no actual mesh is being translated/lerpped (because there isn't an actual mesh being affected unlike in e05.5
		//	computer running out of memory which causes bugs (hhh)
		m_pMeshMngr->AddMeshToRenderList(m_shapeList[i], glm::rotate(m4Offset, 1.5708f, AXIS_X));

		//calculate the current position
		vector3 v3CurrentPos = ZERO_V3;
		matrix4 m4Model;

		//get lerp
		start = pointStorage[i][stopStorage[i]];
		stop = pointStorage[i][(stopStorage[i] + 1) % pointStorage[i].size()];
		v3CurrentPos = glm::lerp(start, stop, percentageOfTime);

		//translate
		m4Model = glm::translate(m4Offset, v3CurrentPos);

		//draw spheres
		m_pMeshMngr->AddSphereToRenderList(m4Model * glm::scale(vector3(0.1)), C_WHITE);

		//if route done
		if (percentageOfTime >= 1.0f)
		{
			//set current stop to next stop
			stopStorage[i]++;
			fTimer = m_pSystem->GetDeltaTime(uClock); //this appearently resets timer
			//if its out boundaries, reset it
			stopStorage[i] %= pointStorage[i].size() - 1;
		}
	}

	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	//release GUI
	ShutdownGUI();
}