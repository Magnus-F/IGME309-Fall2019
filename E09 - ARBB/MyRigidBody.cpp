#include "MyRigidBody.h"
using namespace Simplex;
//Allocation
void MyRigidBody::Init(void)
{
	m_pMeshMngr = MeshManager::GetInstance();
	m_bVisibleBS = false;
	m_bVisibleOBB = true;
	m_bVisibleARBB = true;

	m_fRadius = 0.0f;

	m_v3ColorColliding = C_RED;
	m_v3ColorNotColliding = C_WHITE;

	m_v3Center = ZERO_V3;
	m_v3MinL = ZERO_V3;
	m_v3MaxL = ZERO_V3;

	m_v3MinG = ZERO_V3;
	m_v3MaxG = ZERO_V3;

	m_v3HalfWidth = ZERO_V3;
	m_v3ARBBSize = ZERO_V3;

	m_m4ToWorld = IDENTITY_M4;
}
void MyRigidBody::Swap(MyRigidBody& other)
{
	std::swap(m_pMeshMngr, other.m_pMeshMngr);
	std::swap(m_bVisibleBS, other.m_bVisibleBS);
	std::swap(m_bVisibleOBB, other.m_bVisibleOBB);
	std::swap(m_bVisibleARBB, other.m_bVisibleARBB);

	std::swap(m_fRadius, other.m_fRadius);

	std::swap(m_v3ColorColliding, other.m_v3ColorColliding);
	std::swap(m_v3ColorNotColliding, other.m_v3ColorNotColliding);

	std::swap(m_v3Center, other.m_v3Center);
	std::swap(m_v3MinL, other.m_v3MinL);
	std::swap(m_v3MaxL, other.m_v3MaxL);

	std::swap(m_v3MinG, other.m_v3MinG);
	std::swap(m_v3MaxG, other.m_v3MaxG);

	std::swap(m_v3HalfWidth, other.m_v3HalfWidth);
	std::swap(m_v3ARBBSize, other.m_v3ARBBSize);

	std::swap(m_m4ToWorld, other.m_m4ToWorld);

	std::swap(m_CollidingRBSet, other.m_CollidingRBSet);
}
void MyRigidBody::Release(void)
{
	m_pMeshMngr = nullptr;
	ClearCollidingList();
}
//Accessors
bool MyRigidBody::GetVisibleBS(void) { return m_bVisibleBS; }
void MyRigidBody::SetVisibleBS(bool a_bVisible) { m_bVisibleBS = a_bVisible; }
bool MyRigidBody::GetVisibleOBB(void) { return m_bVisibleOBB; }
void MyRigidBody::SetVisibleOBB(bool a_bVisible) { m_bVisibleOBB = a_bVisible; }
bool MyRigidBody::GetVisibleARBB(void) { return m_bVisibleARBB; }
void MyRigidBody::SetVisibleARBB(bool a_bVisible) { m_bVisibleARBB = a_bVisible; }
float MyRigidBody::GetRadius(void) { return m_fRadius; }
vector3 MyRigidBody::GetColorColliding(void) { return m_v3ColorColliding; }
vector3 MyRigidBody::GetColorNotColliding(void) { return m_v3ColorNotColliding; }
void MyRigidBody::SetColorColliding(vector3 a_v3Color) { m_v3ColorColliding = a_v3Color; }
void MyRigidBody::SetColorNotColliding(vector3 a_v3Color) { m_v3ColorNotColliding = a_v3Color; }
vector3 MyRigidBody::GetCenterLocal(void) { return m_v3Center; }
vector3 MyRigidBody::GetMinLocal(void) { return m_v3MinL; }
vector3 MyRigidBody::GetMaxLocal(void) { return m_v3MaxL; }
vector3 MyRigidBody::GetCenterGlobal(void){	return vector3(m_m4ToWorld * vector4(m_v3Center, 1.0f)); }
vector3 MyRigidBody::GetMinGlobal(void) { return m_v3MinG; }
vector3 MyRigidBody::GetMaxGlobal(void) { return m_v3MaxG; }
vector3 MyRigidBody::GetHalfWidth(void) { return m_v3HalfWidth; }
matrix4 MyRigidBody::GetModelMatrix(void) { return m_m4ToWorld; }
void MyRigidBody::SetModelMatrix(matrix4 a_m4ModelMatrix)
{
	//to save some calculations if the model matrix is the same there is nothing to do here
	if (a_m4ModelMatrix == m_m4ToWorld)
		return;

	m_m4ToWorld = a_m4ModelMatrix;
	
	//your code goes here---------------------
	/*
	X, Y, Z
	m_v3MinL and m_v3MaxL
	Min, Min, Min
	Min, Min, Max
	Min, Max, Min
	Min, Max, Max
	Max, Min, Min
	Max, Min, Max
	Max, Max, Min
	Max, Max, Max
	*/
	//make a vertex array to make things easier on me
	std::vector<glm::vec3> calculationVectors = {
		glm::vec3(m_v3MinL.x, m_v3MinL.y, m_v3MinL.z),
		glm::vec3(m_v3MinL.x, m_v3MinL.y, m_v3MaxL.z),
		glm::vec3(m_v3MinL.x, m_v3MaxL.y, m_v3MinL.z),
		glm::vec3(m_v3MinL.x, m_v3MaxL.y, m_v3MaxL.z),
		glm::vec3(m_v3MaxL.x, m_v3MinL.y, m_v3MinL.z),
		glm::vec3(m_v3MaxL.x, m_v3MinL.y, m_v3MaxL.z),
		glm::vec3(m_v3MaxL.x, m_v3MaxL.y, m_v3MinL.z),
		glm::vec3(m_v3MaxL.x, m_v3MaxL.y, m_v3MaxL.z)
	};

	//set min and max to be first in vector in order to not screw up numbers
	//if the numbers are already less than, as they would be at 0,0,0, then it wouldn't change
	calculationVectors[0] = glm::vec3(m_m4ToWorld * glm::vec4(calculationVectors[0], 1));
	m_v3MinG = calculationVectors[0];
	m_v3MaxG = calculationVectors[0];

	for (int i = 1; i < calculationVectors.size(); i++)
	{
		//Rotate them in world space (multiply by world matrix)
		calculationVectors[i] = glm::vec3(m_m4ToWorld * glm::vec4(calculationVectors[i], 1));
		//Go through each rotated point and find min/max values for each axis; set them to m_v3MinG and m_v3MaxG


		//for min
		if (calculationVectors[i].x < m_v3MinG.x)
			m_v3MinG.x = calculationVectors[i].x;
		if (calculationVectors[i].y < m_v3MinG.y)
			m_v3MinG.y = calculationVectors[i].y;
		if (calculationVectors[i].z < m_v3MinG.z)
			m_v3MinG.z = calculationVectors[i].z;

		//for max
		if (calculationVectors[i].x > m_v3MaxG.x)
			m_v3MaxG.x = calculationVectors[i].x;
		if (calculationVectors[i].y > m_v3MaxG.y)
			m_v3MaxG.y = calculationVectors[i].y;
		if (calculationVectors[i].z > m_v3MaxG.z)
			m_v3MaxG.z = calculationVectors[i].z;
	}


	std::cout << "max" << m_v3MaxG.x << m_v3MaxG.y << m_v3MaxG.z << std::endl;
	std::cout << "min" << m_v3MinG.x << m_v3MinG.y << m_v3MinG.z << std::endl;
	//----------------------------------------

	//we calculate the distance between min and max vectors
	m_v3ARBBSize = m_v3MaxG - m_v3MinG;
}
//The big 3
MyRigidBody::MyRigidBody(std::vector<vector3> a_pointList)
{
	Init();
	//Count the points of the incoming list
	uint uVertexCount = a_pointList.size();

	//If there are none just return, we have no information to create the BS from
	if (uVertexCount == 0)
		return;

	//Max and min as the first vector of the list
	m_v3MaxL = m_v3MinL = a_pointList[0];

	//Get the max and min out of the list
	for (uint i = 1; i < uVertexCount; ++i)
	{
		if (m_v3MaxL.x < a_pointList[i].x) m_v3MaxL.x = a_pointList[i].x;
		else if (m_v3MinL.x > a_pointList[i].x) m_v3MinL.x = a_pointList[i].x;

		if (m_v3MaxL.y < a_pointList[i].y) m_v3MaxL.y = a_pointList[i].y;
		else if (m_v3MinL.y > a_pointList[i].y) m_v3MinL.y = a_pointList[i].y;

		if (m_v3MaxL.z < a_pointList[i].z) m_v3MaxL.z = a_pointList[i].z;
		else if (m_v3MinL.z > a_pointList[i].z) m_v3MinL.z = a_pointList[i].z;
	}

	//with model matrix being the identity, local and global are the same
	m_v3MinG = m_v3MinL;
	m_v3MaxG = m_v3MaxL;

	//with the max and the min we calculate the center
	m_v3Center = (m_v3MaxL + m_v3MinL) / 2.0f;

	//we calculate the distance between min and max vectors
	m_v3HalfWidth = (m_v3MaxL - m_v3MinL) / 2.0f;

	//Get the distance between the center and either the min or the max
	m_fRadius = glm::distance(m_v3Center, m_v3MinL);
}
MyRigidBody::MyRigidBody(MyRigidBody const& other)
{
	m_pMeshMngr = other.m_pMeshMngr;

	m_bVisibleBS = other.m_bVisibleBS;
	m_bVisibleOBB = other.m_bVisibleOBB;
	m_bVisibleARBB = other.m_bVisibleARBB;

	m_fRadius = other.m_fRadius;

	m_v3ColorColliding = other.m_v3ColorColliding;
	m_v3ColorNotColliding = other.m_v3ColorNotColliding;

	m_v3Center = other.m_v3Center;
	m_v3MinL = other.m_v3MinL;
	m_v3MaxL = other.m_v3MaxL;

	m_v3MinG = other.m_v3MinG;
	m_v3MaxG = other.m_v3MaxG;

	m_v3HalfWidth = other.m_v3HalfWidth;
	m_v3ARBBSize = other.m_v3ARBBSize;

	m_m4ToWorld = other.m_m4ToWorld;

	m_CollidingRBSet = other.m_CollidingRBSet;
}
MyRigidBody& MyRigidBody::operator=(MyRigidBody const& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyRigidBody temp(other);
		Swap(temp);
	}
	return *this;
}
MyRigidBody::~MyRigidBody() { Release(); };

//--- other Methods
void MyRigidBody::AddCollisionWith(MyRigidBody* other)
{
	/*
		check if the object is already in the colliding set, if
		the object is already there return with no changes
	*/
	auto element = m_CollidingRBSet.find(other);
	if (element != m_CollidingRBSet.end())
		return;
	// we couldn't find the object so add it
	m_CollidingRBSet.insert(other);
}
void MyRigidBody::RemoveCollisionWith(MyRigidBody* other)
{
	m_CollidingRBSet.erase(other);
}
void MyRigidBody::ClearCollidingList(void)
{
	m_CollidingRBSet.clear();
}
bool MyRigidBody::IsColliding(MyRigidBody* const other)
{
	//check if spheres are colliding
	bool bColliding = true;
	//bColliding = (glm::distance(GetCenterGlobal(), other->GetCenterGlobal()) < m_fRadius + other->m_fRadius);
	//if they are check the Axis Aligned Bounding Box
	if (bColliding) //they are colliding with bounding sphere
	{
		if (this->m_v3MaxG.x < other->m_v3MinG.x) //this to the right of other
			bColliding = false;
		if (this->m_v3MinG.x > other->m_v3MaxG.x) //this to the left of other
			bColliding = false;

		if (this->m_v3MaxG.y < other->m_v3MinG.y) //this below of other
			bColliding = false;
		if (this->m_v3MinG.y > other->m_v3MaxG.y) //this above of other
			bColliding = false;

		if (this->m_v3MaxG.z < other->m_v3MinG.z) //this behind of other
			bColliding = false;
		if (this->m_v3MinG.z > other->m_v3MaxG.z) //this in front of other
			bColliding = false;

		if (bColliding) //they are colliding with bounding box also
		{
			this->AddCollisionWith(other);
			other->AddCollisionWith(this);
		}
		else //they are not colliding with bounding box
		{
			this->RemoveCollisionWith(other);
			other->RemoveCollisionWith(this);
		}
	}
	else //they are not colliding with bounding sphere
	{
		this->RemoveCollisionWith(other);
		other->RemoveCollisionWith(this);
	}
	return bColliding;
}

void MyRigidBody::AddToRenderList(void)
{
	if (m_bVisibleBS)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireSphereToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(vector3(m_fRadius)), C_BLUE_CORNFLOWER);
		else
			m_pMeshMngr->AddWireSphereToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(vector3(m_fRadius)), C_BLUE_CORNFLOWER);
	}
	if (m_bVisibleOBB)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(m_v3HalfWidth * 2.0f), m_v3ColorColliding);
		else
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(m_v3HalfWidth * 2.0f), m_v3ColorNotColliding);
	}
	if (m_bVisibleARBB)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(GetCenterGlobal()) * glm::scale(m_v3ARBBSize), C_YELLOW);
		else
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(GetCenterGlobal()) * glm::scale(m_v3ARBBSize), C_YELLOW);
	}
}