#include "AppClass.h"
void Application::InitVariables(void)
{
	//Make MyMesh object
	m_pMesh = new MyMesh();
	m_pMesh->GenerateCube(1.0f, C_BROWN);

	//Make MyMesh object
	m_pMesh1 = new MyMesh();
	m_pMesh1->GenerateCube(1.0f, C_GREEN_DARK);

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
	//m_pMesh->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3(0.0f, 0.0f, 0.0f))); //base
		
	//drawing space invader
	drawSpaceInvader();

	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
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
	if (m_pMesh != nullptr)
	{
		delete m_pMesh;
		m_pMesh = nullptr;
	}
	SafeDelete(m_pMesh1);
	//release GUI
	ShutdownGUI();
}
void Application::drawSpaceInvader()
{
	//making space invaders thing
	//keep in mind the cubes are one unit across, meaning just i can be used for most of them
	//2 for loops are necessary
	//maybe do if statements for shape?
	//invader alien is 11 units wide and 8 units tall
	//render is upside down!!!
	//0 1 2 3 4 5 6 7 8 9 0
	//_ _ X _ _ _ _ _ X _ _ 8
	//_ _ _ X _ _ _ X _ _ _ 7
	//_ _ X X X X X X X _ _ 6
	//_ X X _ X X X _ X X _ 5
	//X X X X X X X X X X X 4
	//X _ X X X X X X X _ X 3
	//X _ X _ _ _ _ _ X _ X 2
	//_ _ _ X X _ X X _ _ _ 1
	//make antennae first
	m_pMesh1->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3(2, 8.0f, 0.0f)));
	m_pMesh1->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3(3, 7.0f, 0.0f)));
	m_pMesh1->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3(8, 8.0f, 0.0f)));
	m_pMesh1->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3(7, 7.0f, 0.0f)));
	//rest of body
	for (float y = 6; y > 0; y--)
	{
		for (float x = 10; x >= 0; x--)
		{
			if (y == 6)
			{
				for (float xTwo = 2; xTwo < 9; xTwo++)
				{
					m_pMesh1->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3(xTwo, y, 0.0f)));
				}
			}
			else if (y == 5 && x == 7 || y == 5 && x == 3 || y == 5 && x == 0 || y == 5 && x == 10)
			{
				continue;
			}
			else if (y == 3 && x == 1 || y == 3 && x == 9) {
				continue;
			}
			else if (y == 2)
			{
				m_pMesh1->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3(0, y, 0.0f)));
				m_pMesh1->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3(2, y, 0.0f)));
				m_pMesh1->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3(8, y, 0.0f)));
				m_pMesh1->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3(10, y, 0.0f)));
			}
			else if (y == 1)
			{
				m_pMesh1->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3(3, y, 0.0f)));
				m_pMesh1->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3(4, y, 0.0f)));
				m_pMesh1->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3(6, y, 0.0f)));
				m_pMesh1->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3(7, y, 0.0f)));
			}
			else
			{
				m_pMesh1->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3(x, y, 0.0f)));
			}
		}
	}
}