#pragma once
#ifndef __OCTLEAF_H_
#define __OCTLEAF_H_

#include "Definitions.h"
#include "MyRigidBody.h"

#include "ControllerConfiguration.h"
#include "imgui\ImGuiObject.h"
namespace Simplex {
	class OctLeaf
	{
	private:
		std::vector<vector3> points; //4 point locale for each corner of the wirecube (rigidbody)
		MyRigidBody* form; //for checking if the cubes collide and drawing
		std::vector<MyRigidBody*> cubes; //list of all the cubes that will be in teh scene, so we can check if anyone belogs to who
		std::vector<MyRigidBody*> myCubes; //list of the cubes inside of the octleaf
		bool visible; //checks if the cube is visible or not
		//children of the octleaf
		OctLeaf* leftTopF = nullptr;
		OctLeaf* rightTopF = nullptr;
		OctLeaf* leftBottomF = nullptr;
		OctLeaf* rightBottomF = nullptr;
		OctLeaf* leftTopB = nullptr;
		OctLeaf* rightTopB = nullptr;
		OctLeaf* leftBottomB = nullptr;
		OctLeaf* rightBottomB = nullptr;
		uint subNum; //assigned subdivision
	public:
		//rule of three
		//constructor
		OctLeaf(std::vector<vector3>locale, std::vector<MyRigidBody*>models, uint subdivision);
		//copy constructor
		OctLeaf(OctLeaf const& other);
		//copy assignment operator
		OctLeaf& operator=(OctLeaf const& other);
		//destructor
		~OctLeaf();
		//init function
		void Init();
		//fills myCubes according to what cubes are currently inside the form
		void setMyCubes();
		//draws octleaf at smallest stage (if applicable)
		void setVisible();
		//creates new octleaf children according to subdivisions (recursive)
		void Subdivide(uint currentSub, std::vector<vector3> corners);
		//helper function for subdivide
		vector3 findMiddle(vector3 one, vector3 two);
	};

}
#endif //__OCTLEAF_H_

