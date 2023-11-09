#pragma once

#include "../ew/mesh.h"
#include "../ew/transform.h"
#include "../ew/procGen.h"

namespace MyLibrary
{
	enum ringTypes
	{
		TOP_FACE,
		BOTTOM_FACE,
		TOP_EDGE,
		BOTTOM_EDGE,
		ANGLED
	};
	
	ew::MeshData createSphere(float radius, int numSegments);
	ew::MeshData createCylinder(float height, float radius, int numSegements);
	ew::MeshData createPlane(float size, int subDivisions);
	ew::MeshData createTorus(float innerRadius, int outerRadius, int subDivisions);
	void circlePush(std::vector<ew::Vertex>* verticiesList, int subDivisions, float radius, ringTypes ringType, ew::Vec3 posOffset = { 0, 0, 0 }, float angOffset = 0);

	struct Sphere
	{
		float radius = 1;
		int subDivisions = 6;
		ew::Transform transform;
		ew::Mesh getMesh() { return ew::Mesh(ew::createSphere(radius, subDivisions)); }
	};

	struct Cylinder
	{
		float height = 1;
		float radius = 0.75;
		int subDivisions = 6;
		ew::Transform transform;
		ew::Mesh getMesh() { return ew::Mesh(createCylinder(height, radius, subDivisions)); }
	};

	struct Plane
	{
		float size = 1;
		int subDivisions = 8;
		ew::Transform transform;
		ew::Mesh getMesh() { return ew::Mesh(createPlane(size, subDivisions)); }
	};

	struct Cube
	{
		float size = 1;
		ew::Transform transform;
		ew::Mesh getMesh() { return ew::Mesh(ew::createCube(size)); }
	};
}
