#include "MyMesh.h"
void MyMesh::Init(void)
{
	m_bBinded = false;
	m_uVertexCount = 0;

	m_VAO = 0;
	m_VBO = 0;

	m_pShaderMngr = ShaderManager::GetInstance();
}
void MyMesh::Release(void)
{
	m_pShaderMngr = nullptr;

	if (m_VBO > 0)
		glDeleteBuffers(1, &m_VBO);

	if (m_VAO > 0)
		glDeleteVertexArrays(1, &m_VAO);

	m_lVertex.clear();
	m_lVertexPos.clear();
	m_lVertexCol.clear();
}
MyMesh::MyMesh()
{
	Init();
}
MyMesh::~MyMesh() { Release(); }
MyMesh::MyMesh(MyMesh& other)
{
	m_bBinded = other.m_bBinded;

	m_pShaderMngr = other.m_pShaderMngr;

	m_uVertexCount = other.m_uVertexCount;

	m_VAO = other.m_VAO;
	m_VBO = other.m_VBO;
}
MyMesh& MyMesh::operator=(MyMesh& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyMesh temp(other);
		Swap(temp);
	}
	return *this;
}
void MyMesh::Swap(MyMesh& other)
{
	std::swap(m_bBinded, other.m_bBinded);
	std::swap(m_uVertexCount, other.m_uVertexCount);

	std::swap(m_VAO, other.m_VAO);
	std::swap(m_VBO, other.m_VBO);

	std::swap(m_lVertex, other.m_lVertex);
	std::swap(m_lVertexPos, other.m_lVertexPos);
	std::swap(m_lVertexCol, other.m_lVertexCol);

	std::swap(m_pShaderMngr, other.m_pShaderMngr);
}
void MyMesh::CompleteMesh(vector3 a_v3Color)
{
	uint uColorCount = m_lVertexCol.size();
	for (uint i = uColorCount; i < m_uVertexCount; ++i)
	{
		m_lVertexCol.push_back(a_v3Color);
	}
}
void MyMesh::AddVertexPosition(vector3 a_v3Input)
{
	m_lVertexPos.push_back(a_v3Input);
	m_uVertexCount = m_lVertexPos.size();
}
void MyMesh::AddVertexColor(vector3 a_v3Input)
{
	m_lVertexCol.push_back(a_v3Input);
}
void MyMesh::CompileOpenGL3X(void)
{
	if (m_bBinded)
		return;

	if (m_uVertexCount == 0)
		return;

	CompleteMesh();

	for (uint i = 0; i < m_uVertexCount; i++)
	{
		//Position
		m_lVertex.push_back(m_lVertexPos[i]);
		//Color
		m_lVertex.push_back(m_lVertexCol[i]);
	}
	glGenVertexArrays(1, &m_VAO);//Generate vertex array object
	glGenBuffers(1, &m_VBO);//Generate Vertex Buffered Object

	glBindVertexArray(m_VAO);//Bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//Bind the VBO
	glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);//Generate space for the VBO

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

	m_bBinded = true;

	glBindVertexArray(0); // Unbind VAO
}
void MyMesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel)
{
	// Use the buffer and shader
	GLuint nShader = m_pShaderMngr->GetShaderID("Basic");
	glUseProgram(nShader); 

	//Bind the VAO of this object
	glBindVertexArray(m_VAO);

	// Get the GPU variables by their name and hook them to CPU variables
	GLuint MVP = glGetUniformLocation(nShader, "MVP");
	GLuint wire = glGetUniformLocation(nShader, "wire");

	//Final Projection of the Camera
	matrix4 m4MVP = a_mProjection * a_mView * a_mModel;
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(m4MVP));
	
	//Solid
	glUniform3f(wire, -1.0f, -1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);  

	//Wire
	glUniform3f(wire, 1.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.f, -1.f);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);
	glDisable(GL_POLYGON_OFFSET_LINE);

	glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}
void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft)
{
	//C
	//| \
	//A--B
	//This will make the triangle A->B->C 
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);
}
void MyMesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight)
{
	//C--D
	//|  |
	//A--B
	//This will make the triangle A->B->C and then the triangle C->B->D
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);

	AddVertexPosition(a_vTopLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopRight);
}
void MyMesh::GenerateCircle(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	Release();
	Init();

	if (a_fRadius < 0.01f)
	{
		a_fRadius = 0.01f;
	}

	if (a_nSubdivisions < 3) {
		a_nSubdivisions = 3;
	}
	else if (a_nSubdivisions > 360) {
		a_nSubdivisions = 360;
	}
	//start with the base point (vector made with center (0,0,0) and fRadius)
	vector3 base = vector3(a_fRadius, 0, 0);
	vector3 next; //would help during for loop later
	//making everything a float below because otherwise it will round to the nearest integer and cause peices in the generated image to be missing
	float degreeInterval = 360.0f / (float)a_nSubdivisions; //this gets angle that all triangles will have at center
	vector3 center = vector3(0, 0, 0); //not necessary but good for visualization
	//from that we need to account for the other vectors using the angle of the pretend triangle (made with nSubdivisions/360)
	//for every new point it would be basically base * (tangent rule (?) with angle * i)
	for (int i = 0; i < a_nSubdivisions + 1; i++)
	{
		//calculate necessary numbers
		next = base;
		float radianCalc = (degreeInterval * 3.14159265359 * i) / 180.0f;
		float x = cos(radianCalc) * a_fRadius;
		float y = sin(radianCalc) * a_fRadius;
		//flop em all into next
		next = vector3(x, y, 0);
		//draw the triangle
		AddTri(center, base, next);
		//forget previous base
		base = next;
	}

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCube(float a_fSize, vector3 a_v3Color)
{
	if (a_fSize < 0.01f)
		a_fSize = 0.01f;

	Release();
	Init();

	float fValue = a_fSize * 0.5f;
	//3--2
	//|  |
	//0--1

	vector3 point0(-fValue,-fValue, fValue); //0
	vector3 point1( fValue,-fValue, fValue); //1
	vector3 point2( fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue,-fValue,-fValue); //4
	vector3 point5( fValue,-fValue,-fValue); //5
	vector3 point6( fValue, fValue,-fValue); //6
	vector3 point7(-fValue, fValue,-fValue); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color)
{
	Release();
	Init();

	vector3 v3Value = a_v3Dimensions * 0.5f;
	//3--2
	//|  |
	//0--1
	vector3 point0(-v3Value.x, -v3Value.y, v3Value.z); //0
	vector3 point1(v3Value.x, -v3Value.y, v3Value.z); //1
	vector3 point2(v3Value.x, v3Value.y, v3Value.z); //2
	vector3 point3(-v3Value.x, v3Value.y, v3Value.z); //3

	vector3 point4(-v3Value.x, -v3Value.y, -v3Value.z); //4
	vector3 point5(v3Value.x, -v3Value.y, -v3Value.z); //5
	vector3 point6(v3Value.x, v3Value.y, -v3Value.z); //6
	vector3 point7(-v3Value.x, v3Value.y, -v3Value.z); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	//start with the base point (vector made with center (0,0,0) and fRadius)
	vector3 base = vector3(a_fRadius, 0, 0);
	vector3 top = vector3(0, a_fHeight, 0);
	vector3 next; //would help during for loop later
	//making everything a float below because otherwise it will round to the nearest integer and cause peices in the generated image to be missing
	float degreeInterval = 360.0f / (float)a_nSubdivisions; //this gets angle that all triangles will have at center
	vector3 center = vector3(0, 0, 0); //not necessary but good for visualization
	//from that we need to account for the other vectors using the angle of the pretend triangle (made with nSubdivisions/360)
	//for every new point it would be basically base * (tangent rule (?) with angle * i)
	//remember, we're dealing with the x and z axis, rather than the x and y like before
	//also we woudl need to add two triangles, one for the base circle and the other for the uprooting triangle
	for (int i = 0; i < a_nSubdivisions + 1; i++)
	{
		//calculate necessary numbers
		next = base;
		float radianCalc = (degreeInterval * 3.14159265359 * i) / 180.0f;
		float x = cos(radianCalc) * a_fRadius;
		float z = sin(radianCalc) * a_fRadius;
		//flop em all into next
		next = vector3(x, 0, z);
		//draw the triangle
		AddTri(base, next, center); //circle
		AddTri(base, top, next); //top bit
		//forget previous base
		base = next;
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	//with this one we'll need two circles generated as well as a quad that stretches between the two

	//keep in mind
	//C--D
	//|  |
	//A--B
	//This will make the triangle A->B->C and then the triangle C->B->D

	//start with the base point (vector made with center (0,0,0) and fRadius)
	vector3 baseBottom = vector3(a_fRadius, 0, 0);
	vector3 baseTop = vector3(a_fRadius, a_fHeight, 0);
	vector3 nextBottom; //would help during for loop later
	vector3 nextTop; //would help during for loop later
	//making everything a float below because otherwise it will round to the nearest integer and cause peices in the generated image to be missing
	float degreeInterval = 360.0f / (float)a_nSubdivisions; //this gets angle that all triangles will have at center
	vector3 centerBottom = vector3(0, 0, 0); //not necessary but good for visualization
	vector3 centerTop = vector3(0, a_fHeight, 0); //not necessary but good for visualization
	//from that we need to account for the other vectors using the angle of the pretend triangle (made with nSubdivisions/360)
	//for every new point it would be basically base * (tangent rule (?) with angle * i)
	//remember, we're dealing with the x and z axis, rather than the x and y like before
	//also we woudl need to add two triangles, one for the base circle and the other for the uprooting triangle
	for (int i = 0; i < a_nSubdivisions + 1; i++)
	{
		//calculate necessary numbers
		nextBottom = baseBottom;
		float radianCalc = (degreeInterval * 3.14159265359 * i) / 180.0f;
		float z = cos(radianCalc) * a_fRadius;
		float y = sin(radianCalc) * a_fRadius;
		//flop em all into next
		nextBottom = vector3(z, 0, y);
		nextTop = vector3(z, a_fHeight, y);
		//draw the triangle
		AddTri(baseBottom, nextBottom, centerBottom); //bottom circle
		AddTri(nextTop, baseTop, centerTop); //top circle
		AddQuad(nextBottom, baseBottom, nextTop, baseTop);
		//forget previous base
		baseBottom = nextBottom;
		baseTop = nextTop;
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	//with this one we'll need four points (base, next, innerBase, innerNext) for both sides, and two rows of quads to stretch between the two

	//keep in mind
	//C--D
	//|  |
	//A--B
	//This will make the triangle A->B->C and then the triangle C->B->D

	//start with the various points used
	//outer
	vector3 baseBottomO = vector3(a_fOuterRadius, 0, 0);
	vector3 baseTopO = vector3(a_fOuterRadius, a_fHeight, 0);
	vector3 nextBottomO; //would help during for loop later
	vector3 nextTopO; //would help during for loop later

	//inner
	vector3 baseBottomI = vector3(a_fInnerRadius, 0, 0);
	vector3 baseTopI = vector3(a_fInnerRadius, a_fHeight, 0);
	vector3 nextBottomI; //would help during for loop later
	vector3 nextTopI; //would help during for loop later


	//making everything a float below because otherwise it will round to the nearest integer and cause peices in the generated image to be missing
	float degreeInterval = 360.0f / (float)a_nSubdivisions; //this gets angle that all triangles will have at center
	//from that we need to account for the other vectors using the angle of the pretend triangle (made with nSubdivisions/360)
	//for every new point it would be basically base * (tangent rule (?) with angle * i)
	//remember, we're dealing with the x and z axis, rather than the x and y like before
	//also we woudl need to add two triangles, one for the base circle and the other for the uprooting triangle
	for (int i = 0; i < a_nSubdivisions + 1; i++)
	{
		//calculate NUMBERS
		nextBottomO = baseBottomO;
		nextBottomI = baseBottomI;
		float radianCalc = (degreeInterval * 3.14159265359 * i) / 180.0f;
		//calculate outeer numbers
		float x = cos(radianCalc) * a_fOuterRadius;
		float z = sin(radianCalc) * a_fOuterRadius;
		//flop em all into next
		nextBottomO = vector3(x, 0, z);
		nextTopO = vector3(x, a_fHeight, z);
		//calculate inner numbers
		x = cos(radianCalc) * a_fInnerRadius;
		z = sin(radianCalc) * a_fInnerRadius;
		//flop em all into next
		nextBottomI = vector3(x, 0, z);
		nextTopI = vector3(x, a_fHeight, z);


		//draw the quads
		//AddTri(baseBottomO, nextBottomO, centerBottom); //bottom circle
		//AddTri(nextTopO, baseTopO, centerTop); //top circle
		AddQuad(nextBottomI, baseBottomI, nextBottomO, baseBottomO); //top donut
		AddQuad(baseTopI, nextTopI, baseTopO, nextTopO); //bottom donut
		AddQuad(nextBottomO, baseBottomO, nextTopO, baseTopO); //outer quad
		AddQuad(baseBottomI, nextBottomI, baseTopI, nextTopI); //inner quad
		//forget previous base
		baseBottomO = nextBottomO;
		baseTopO = nextTopO;
		baseBottomI = nextBottomI;
		baseTopI = nextTopI;
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsA, int a_nSubdivisionsB, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_nSubdivisionsA < 3)
		a_nSubdivisionsA = 3;
	if (a_nSubdivisionsA > 360)
		a_nSubdivisionsA = 360;

	if (a_nSubdivisionsB < 3)
		a_nSubdivisionsB = 3;
	if (a_nSubdivisionsB > 360)
		a_nSubdivisionsB = 360;

	Release();
	Init();

	// Replace this with your code
	GenerateCube(a_fOuterRadius * 2.0f, a_v3Color);
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 1)
	{
		GenerateCube(a_fRadius * 2.0f, a_v3Color);
		return;
	}
	if (a_nSubdivisions > 6)
		a_nSubdivisions = 6;

	Release();
	Init();

	// Replace this with your code
	//keep in mind:
	//radius is referring to the radius at the sphere's thickest
	//subdivisions (in parameters) are referring to the verticle subdivisions
	int horizontalSubdivisions = a_nSubdivisions * 2;
	int infoMatrix[50][3];

	/*
	//with this one we'll need four points (base, next, innerBase, innerNext) for both sides, and two rows of quads to stretch between the two

	//keep in mind
	//C--D
	//|  |
	//A--B
	//This will make the triangle A->B->C and then the triangle C->B->D

	//start with the various points used
	//outer
	vector3 baseBottomO = vector3(a_fRadius, 0, 0);
	vector3 baseTopO = vector3(a_fRadius, 0, 0);
	vector3 nextBottomO; //would help during for loop later
	vector3 nextTopO; //would help during for loop later

	//inner
	vector3 baseBottomI = vector3(a_fRadius, 0, 0);
	vector3 baseTopI = vector3(a_fRadius, 0, 0);
	vector3 nextBottomI; //would help during for loop later
	vector3 nextTopI; //would help during for loop later


	//making everything a float below because otherwise it will round to the nearest integer and cause peices in the generated image to be missing
	float degreeInterval = 360.0f / (float)a_nSubdivisions; //this gets angle that all triangles will have at center
	//from that we need to account for the other vectors using the angle of the pretend triangle (made with nSubdivisions/360)
	//for every new point it would be basically base * (tangent rule (?) with angle * i)
	//remember, we're dealing with the x and z axis, rather than the x and y like before
	//also we woudl need to add two triangles, one for the base circle and the other for the uprooting triangle
	for (int i = 0; i < a_nSubdivisions + 1; i++)
	{
		//calculate NUMBERS
		nextBottomO = baseBottomO;
		nextBottomI = baseBottomI;
		float radianCalc = (degreeInterval * 3.14159265359 * i) / 180.0f;
		//calculate outeer numbers
		float x = cos(radianCalc) * a_fRadius;
		float z = sin(radianCalc) * a_fRadius;
		//flop em all into next
		nextBottomO = vector3(x, 0, z);
		nextTopO = vector3(x, 0, z);
		//calculate inner numbers
		x = cos(radianCalc) * a_fRadius;
		z = sin(radianCalc) * a_fRadius;
		//flop em all into next
		nextBottomI = vector3(x, 0, z);
		nextTopI = vector3(x, 0, z);


		//draw the quads
		//AddTri(baseBottomO, nextBottomO, centerBottom); //bottom circle
		//AddTri(nextTopO, baseTopO, centerTop); //top circle
		AddQuad(nextBottomI, baseBottomI, nextBottomO, baseBottomO); //top donut
		AddQuad(baseTopI, nextTopI, baseTopO, nextTopO); //bottom donut
		//forget previous base
		baseBottomO = nextBottomO;
		baseTopO = nextTopO;
		baseBottomI = nextBottomI;
		baseTopI = nextTopI;
	}
	*/

	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}