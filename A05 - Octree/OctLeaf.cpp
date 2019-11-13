#include "OctLeaf.h"
using namespace Simplex;

void Simplex::OctLeaf::Init()
{
	visible = true;
	myCubes = {};
	setMyCubes();
}

Simplex::OctLeaf::OctLeaf(std::vector<vector3> locale, std::vector<MyRigidBody*> models, uint subdivision)
{
	cubes = models;
	subNum = subdivision;
	points = locale;
	form = new MyRigidBody(points);
	Init();
}

Simplex::OctLeaf::OctLeaf(OctLeaf const& other)
{
	cubes = other.cubes;
	subNum = other.subNum;
	points = other.points;
	form = other.form;
	myCubes = other.myCubes;
	visible = other.visible;
}

OctLeaf& Simplex::OctLeaf::operator=(OctLeaf const& other)
{
	// TODO: insert return statement here
	if (this != &other)
	{
		Init();
		cubes = other.cubes;
		subNum = other.subNum;
		points = other.points;
		form = other.form;
		myCubes = other.myCubes;
		visible = other.visible;
	}
	return *this;
}

//finish later
Simplex::OctLeaf::~OctLeaf()
{
	SafeDelete(form);
	form = nullptr;
	myCubes.clear();
}

void Simplex::OctLeaf::setMyCubes()
{
	for each (MyRigidBody *cube in cubes)
	{
		if (form->IsColliding(cube))
		{
			myCubes.push_back(cube);
		}
	}
}

void Simplex::OctLeaf::setVisible()
{
	form->AddToRenderList();
	std::vector<OctLeaf*> leaves = {
		leftTopF,
		rightTopF,
		leftBottomF,
		rightBottomF,
		leftTopB,
		rightTopB,
		leftBottomB,
		rightBottomB
	};
	for (int i = 0; i < leaves.size(); i++)
	{
		if (leaves[i] != nullptr) //if there is something there
		{
			form->AddToRenderList();
			setVisible();
		}
	}
	return;
}

void Simplex::OctLeaf::Subdivide(uint currentSub, std::vector<vector3> corners)
{
	if (currentSub >= subNum || currentSub == 1 || currentSub == 0) //if we're at a too large subdivision return
	{
		return;
	}
	else
	{
		//first add one to teh current sub
		uint tempSub = currentSub + 1;
		//then for every child, create a new octleaf with an altered corners vector
		std::vector<vector3> temp = { //top left front
			//square one
			corners[1],
			findMiddle(corners[0], corners[1]),
			findMiddle(corners[0], corners[2]),
			findMiddle(corners[0], corners[3]),
			//square two
			findMiddle(corners[0],corners[4]),
			findMiddle(corners[1],corners[4]),
			findMiddle(corners[2], corners[4]),
			findMiddle(corners[3],corners[4])
		};

		leftTopF = new OctLeaf(temp, myCubes, tempSub);
		leftTopF->Subdivide(tempSub, temp);

		temp = { //top right front
			//square one
			findMiddle(corners[0], corners[3]),
			findMiddle(corners[0], corners[2]),
			findMiddle(corners[2], corners[3]),
			corners[3],
			//square two
			findMiddle(corners[3],corners[4]),
			findMiddle(corners[2],corners[4]),
			findMiddle(corners[2], corners[7]),
			findMiddle(corners[3],corners[7])
		};
		std::cout << temp[6].x << std::endl;;
		rightTopF = new OctLeaf(temp, myCubes, tempSub);
		rightTopF->Subdivide(tempSub, temp);

		temp = { //bottom left front
			//square one
			findMiddle(corners[0], corners[1]),
			corners[1],
			findMiddle(corners[1], corners[2]),
			findMiddle(corners[0], corners[2]),
			//square two
			findMiddle(corners[1],corners[4]),
			findMiddle(corners[1],corners[5]),
			findMiddle(corners[2], corners[5]),
			findMiddle(corners[2],corners[4])
		};

		leftBottomF = new OctLeaf(temp, myCubes, tempSub);
		leftBottomF->Subdivide(tempSub, temp);

		temp = { //bottom right front
			//square one
			findMiddle(corners[0], corners[3]),
			findMiddle(corners[1], corners[2]),
			corners[2],
			findMiddle(corners[2], corners[3]),
			//square two
			findMiddle(corners[2],corners[4]),
			findMiddle(corners[1],corners[6]),
			findMiddle(corners[2], corners[6]),
			findMiddle(corners[2],corners[7])
		};

		rightBottomF = new OctLeaf(temp, myCubes, tempSub);
		rightBottomF->Subdivide(tempSub, temp);

		temp = { //top left back
			//square one
			findMiddle(corners[0], corners[4]),
			findMiddle(corners[0], corners[5]),
			findMiddle(corners[0], corners[6]),
			findMiddle(corners[0],corners[7]),
			//square two
			corners[5],
			findMiddle(corners[1],corners[4]),
			findMiddle(corners[2], corners[4]),
			findMiddle(corners[4],corners[7])
		};

		leftTopB = new OctLeaf(temp, myCubes, tempSub);
		leftTopB->Subdivide(tempSub, temp);

		temp = { //bottom left back
			//square one
			findMiddle(corners[4], corners[5]),
			corners[5],
			findMiddle(corners[5], corners[6]),
			findMiddle(corners[4], corners[6]),
			//square two
			findMiddle(corners[1],corners[4]),
			findMiddle(corners[1],corners[5]),
			findMiddle(corners[2], corners[5]),
			findMiddle(corners[2],corners[4])
		};

		leftBottomB = new OctLeaf(temp, myCubes, tempSub);
		leftBottomB->Subdivide(tempSub, temp);

		temp = { //top right back
			//square one
			findMiddle(corners[3], corners[4]),
			findMiddle(corners[2], corners[4]),
			findMiddle(corners[2], corners[7]),
			findMiddle(corners[3],corners[7]),
			//square two
			findMiddle(corners[4],corners[7]),
			findMiddle(corners[4], corners[6]),
			findMiddle(corners[6],corners[7]),
			corners[7]
		};

		rightTopB = new OctLeaf(temp, myCubes, tempSub);
		rightTopB->Subdivide(tempSub, temp);

		temp = { //bottom right back
			//square one
			findMiddle(corners[2], corners[4]),
			findMiddle(corners[2], corners[5]),
			findMiddle(corners[2], corners[6]),
			findMiddle(corners[2],corners[7]),
			//square two
			findMiddle(corners[4],corners[6]),
			findMiddle(corners[5], corners[6]),
			corners[5],
			findMiddle(corners[6],corners[7])
		};

		rightBottomB = new OctLeaf(temp, myCubes, tempSub);
		rightBottomB->Subdivide(tempSub, temp);
		//then after every child is made, call this method again
	}
}

vector3 Simplex::OctLeaf::findMiddle(vector3 one, vector3 two)
{
	vector3 temp(0, 0, 0);
	temp.x = (one.x + two.x) / 4;
	temp.y = (one.y + two.y) / 4;
	temp.z = (one.z + two.z) / 4;
	return temp;
}
