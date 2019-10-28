#include "MyEntityManager.h"
using namespace Simplex;
//  MyEntityManager
MyEntityManager* MyEntityManager::m_pInstance = nullptr;
void MyEntityManager::Init(void)
{
	//instanciate as null
	m_pInstance = nullptr;
	m_uEntityCount = 0;
	m_entityList.clear();
}
void MyEntityManager::Release(void)
{
	//clear for everything then reset
	for (int i = 0; i < m_entityList.size(); i++)
	{
		SafeDelete(m_entityList[i]);
	}
	m_uEntityCount = 0;
	m_entityList.clear();
}
MyEntityManager* MyEntityManager::GetInstance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = new MyEntityManager();
	}
	return m_pInstance;
}
void MyEntityManager::ReleaseInstance()
{
	if (m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}
int Simplex::MyEntityManager::GetEntityIndex(String a_sUniqueID)
{
	for (int i = 0; i < m_entityList.size(); i++)
	{
		if (m_entityList[i]->GetUniqueID() == a_sUniqueID)
		{
			return i; //might have to make this i-1 to account for it not beginning at 0
		}
	}
	return -1;
}
//Accessors
Model* Simplex::MyEntityManager::GetModel(uint a_uIndex)
{
	if (isListEmpty(a_uIndex))
		return nullptr;

	return m_entityList[a_uIndex]->GetModel();
}
Model* Simplex::MyEntityManager::GetModel(String a_sUniqueID)
{
	return m_entityList[GetEntityIndex(a_sUniqueID)]->GetModel();
}
RigidBody* Simplex::MyEntityManager::GetRigidBody(uint a_uIndex)
{
	if (isListEmpty(a_uIndex))
		return nullptr;

	return m_entityList[a_uIndex]->GetRigidBody();
}
RigidBody* Simplex::MyEntityManager::GetRigidBody(String a_sUniqueID)
{
	return m_entityList[GetEntityIndex(a_sUniqueID)]->GetRigidBody();
}
matrix4 Simplex::MyEntityManager::GetModelMatrix(uint a_uIndex)
{
	if (isListEmpty(a_uIndex))
		return IDENTITY_M4;

	return m_entityList[a_uIndex]->GetModelMatrix();
}
matrix4 Simplex::MyEntityManager::GetModelMatrix(String a_sUniqueID)
{
	return m_entityList[GetEntityIndex(a_sUniqueID)]->GetModelMatrix();
}
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, String a_sUniqueID)
{
	m_entityList[GetEntityIndex(a_sUniqueID)]->SetModelMatrix(a_m4ToWorld);
}
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, uint a_uIndex)
{
	if (isListEmpty(a_uIndex))
		return;

	m_entityList[a_uIndex]->SetModelMatrix(a_m4ToWorld);
}
//The big 3
MyEntityManager::MyEntityManager(){Init();}
MyEntityManager::MyEntityManager(MyEntityManager const& other){ }
MyEntityManager& MyEntityManager::operator=(MyEntityManager const& other) { return *this; }
MyEntityManager::~MyEntityManager(){Release();};
// other methods
void Simplex::MyEntityManager::Update(void)
{
	//check if each entity is colliding with one another
	for (int i = 0; i < m_entityList.size(); i++)
	{
		for (int j = 0; j < m_entityList.size(); j++)
		{
			m_entityList[j]->IsColliding(m_entityList[i]);
		}
	}
}
void Simplex::MyEntityManager::AddEntity(String a_sFileName, String a_sUniqueID)
{
	//to make it easier make a temporary
	MyEntity* tempEntitity = new MyEntity(a_sFileName, a_sUniqueID);
	//add to list and increase list size
	m_entityList.push_back(tempEntitity);
	m_uEntityCount = m_entityList.size();
}
void Simplex::MyEntityManager::RemoveEntity(uint a_uIndex)
{
	if (isListEmpty(a_uIndex))
		return;

	//otherwise move to delete it
	//first swap it to make it easier to pop back
	std::swap(m_entityList[a_uIndex], m_entityList[m_uEntityCount - 1]);
	//then delete
	SafeDelete(m_entityList[m_uEntityCount - 1]);
	m_entityList.pop_back();
	m_uEntityCount--;
}
void Simplex::MyEntityManager::RemoveEntity(String a_sUniqueID)
{
	//first get index
	int index = GetEntityIndex(a_sUniqueID);
	//then just call the other method
	RemoveEntity(index);
}
String Simplex::MyEntityManager::GetUniqueID(uint a_uIndex)
{
	if (isListEmpty(a_uIndex))
		return "String Not Available";

	return m_entityList[a_uIndex]->GetUniqueID();
}
MyEntity* Simplex::MyEntityManager::GetEntity(uint a_uIndex)
{
	if (isListEmpty(a_uIndex))
		return nullptr;

	return m_entityList[a_uIndex];
}
void Simplex::MyEntityManager::AddEntityToRenderList(uint a_uIndex, bool a_bRigidBody)
{
	if (isListEmpty(a_uIndex))
		return;

	//else just do the specified one
	m_entityList[a_uIndex]->AddToRenderList(a_bRigidBody);
}
void Simplex::MyEntityManager::AddEntityToRenderList(String a_sUniqueID, bool a_bRigidBody)
{
	//first get index
	int index = GetEntityIndex(a_sUniqueID);
	//then just call the other method
	AddEntityToRenderList(index,a_bRigidBody);
}

bool Simplex::MyEntityManager::isListEmpty(uint index)
{
	if (m_entityList.size() == 0)
	{
		return true;
	}
	//also if out of bounds then make it last one
	if (index >= m_uEntityCount)
	{
		index = m_uEntityCount - 1;
	}
	//finally, if the index given is negative one then return true
	if (index < 0)
	{
		return true;
	}
	return false;
}
