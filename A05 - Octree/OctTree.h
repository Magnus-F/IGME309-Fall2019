#pragma once
#ifndef __OCTTREE_H_
#define __OCTTREE_H_

#include "Definitions.h"

#include "ControllerConfiguration.h"
#include "imgui\ImGuiObject.h"

#include "OctLeaf.h"
namespace Simplex {
class OctTree
{
private:
	uint subdivisions; //how many layers will the octtree have
	std::vector<OctLeaf> leaves; //a vector to keep track of the leaves generated
public:
	//rule of three (do later IDK if this class is even needed)
	//init
	void Init();
	//creates new leaves according to subdivision num
	void Subdivide(uint subdiv);
};

}
#endif //__OCTTREE_H_

