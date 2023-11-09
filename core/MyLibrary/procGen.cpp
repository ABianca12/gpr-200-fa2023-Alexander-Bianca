#include "procGen.h"

using namespace std;

namespace MyLibrary
{
	ew::MeshData createSphere(float radius, int numSegments)
	{
		ew::MeshData mesh;
		float thetaStep = ew::TAU / numSegments;
		float phiStep = ew::PI / numSegments;

		for (int row = 0; row <= numSegments; row++)
		{
			float phi = row * phiStep;
			for (int col = 0; col <= numSegments; col++)
			{
				ew::Vertex v;
				float theta = col * thetaStep;
				v.pos.x = radius * sin(phi) * sin(theta);
				v.pos.y = radius * cos(phi);
				v.pos.z = radius * sin(phi) * cos(theta);
				v.uv = { (float)col / numSegments, (float)row / numSegments };
				v.normal = ew::Normalize(v.pos);
				mesh.vertices.push_back(v);
			}
		}

		int poleStart = 0;
		int sideStart = numSegments + 1;
		for (int i = 0; i < numSegments; i++)
		{
			mesh.indices.push_back(sideStart + i + 1);
			mesh.indices.push_back(poleStart + i);
			mesh.indices.push_back(sideStart + i);
		}

		int columns = numSegments + 1;
		for (int row = 1; row < numSegments - 1; row++)
		{
			for (int col = 0; col < numSegments; col++)
			{
				int start = row * columns + col;

				mesh.indices.push_back(start + columns);
				mesh.indices.push_back(start + 1);
				mesh.indices.push_back(start);

				mesh.indices.push_back(start + columns);
				mesh.indices.push_back(start + columns + 1);
				mesh.indices.push_back(start + 1);
			}
		}

		poleStart = numSegments * (numSegments + 1);
		sideStart = (numSegments * numSegments) - 1;
		for (int i = 0; i < numSegments; i++)
		{
			mesh.indices.push_back(sideStart + i);
			mesh.indices.push_back(poleStart + i);
			mesh.indices.push_back(sideStart + i + 1);
		}

		return mesh;
	}

	ew::MeshData createCylinder(float height, float radius, int numSegments)
	{
		ew::MeshData mesh;
		float topY = height / 2;
		float bottomY = -topY;

		ew::Vertex topPoint;
		topPoint.pos = { 0, topY, 0 };
		topPoint.normal = { 0, 1, 0 };
		topPoint.uv = { 0.5, 0.5 };
		mesh.vertices.push_back(topPoint);

		circlePush(&mesh.vertices, numSegments, radius, TOP_FACE, { 0, topY, 0 });

		int startSideIndicies = mesh.vertices.size();
		circlePush(&mesh.vertices, numSegments, radius, TOP_EDGE, { 0, topY, 0 });

		circlePush(&mesh.vertices, numSegments, radius, BOTTOM_EDGE, { 0, bottomY, 0 });

		int startBottomIndicies = mesh.vertices.size();
		circlePush(&mesh.vertices, numSegments, radius, BOTTOM_FACE, { 0, bottomY, 0 });

		ew::Vertex bottomPoint;
		bottomPoint.pos = { 0, bottomY, 0 };
		bottomPoint.normal = { 0, -1, 0 };
		bottomPoint.uv = { 0.5, 0.5 };
		mesh.vertices.push_back(bottomPoint);

		int startIndex = 1;
		int centerIndex = 0;
		for (int i = 0; i <= numSegments; i++)
		{
			mesh.indices.push_back(startIndex + i);
			mesh.indices.push_back(centerIndex);
			mesh.indices.push_back(startIndex + i + 1);
		}

		centerIndex = mesh.vertices.size() - 1;
		for (int i = 0; i <= numSegments; i++)
		{
			mesh.indices.push_back(startBottomIndicies + i + 1);
			mesh.indices.push_back(centerIndex);
			mesh.indices.push_back(startBottomIndicies + i);
		}

		int columns = numSegments + 1;
		for (int i = 0; i < columns; i++)
		{
			int start = startSideIndicies + i;

			mesh.indices.push_back(start);
			mesh.indices.push_back(start + 1);
			mesh.indices.push_back(start + columns);

			mesh.indices.push_back(start + 1);
			mesh.indices.push_back(start + columns + 1);
			mesh.indices.push_back(start + columns);
		}
		return mesh;
	}

	ew::MeshData createPlane(float size, int subDivisions)
	{
		ew::MeshData mesh;
		int columns = subDivisions + 1;

		for (int i = 0; i <= subDivisions; i++)
		{
			for (int j = 0; j <= subDivisions; j++)
			{
				ew::Vertex v;
				v.pos.x = j * (size / j);
				v.pos.y = 0;
				v.pos.z = i * (size / j);
				v.normal = ew::Vec3(0, 1, 0);
				v.uv = { ((float)j / (columns - 1)), ((float)i / (columns - 1)) };
				mesh.vertices.push_back(v);
			}
		}

		for (int i = 0; i < subDivisions; i++)
		{
			for (int j = 0; j < subDivisions; j++)
			{
				int start = i * columns + j;

				mesh.indices.push_back(start + columns);
				mesh.indices.push_back(start + columns + 1);
				mesh.indices.push_back(start);

				mesh.indices.push_back(start + columns + 1);
				mesh.indices.push_back(start + 1);
				mesh.indices.push_back(start);
			}
		}
		return mesh;
	}

	void circlePush(std::vector<ew::Vertex>* verticiesList, int subDivisions, float radius, ringTypes ringType, ew::Vec3 posOffset, float angOffset)
	{
		float thetaStep = ew::TAU / subDivisions;
		for (int i = 0; i <= subDivisions; i++)
		{
			ew::Vertex v;
			float theta = i * thetaStep;
			v.pos.x = (cos(theta) * radius) + posOffset.x;
			v.pos.y = posOffset.y;
			v.pos.z = (sin(theta) * radius) + posOffset.z;
			switch (ringType)
			{
			case TOP_FACE:
				v.normal = { 0, 1, 0 };
				v.uv = { (cos(theta) + 1) / 2, (sin(theta) + 1) / 2 };
				break;
			case BOTTOM_FACE:
				v.normal = { 0, -1, 0 };
				v.uv = { (cos(theta) + 1) / 2, (sin(theta) + 1) / 2 };
				break;
			case TOP_EDGE:
				v.normal = { cos(theta), 0, sin(theta) };
				v.uv = { theta / ew::TAU, 1 };
				break;
			case BOTTOM_EDGE:
				v.normal = { cos(theta), 0, sin(theta) };
				v.uv = { theta / ew::TAU, 0 };
				break;
			case ANGLED:
				v.pos.x = v.pos.x * cos(angOffset) - v.pos.y * sin(angOffset);
				v.pos.y = v.pos.x * sin(angOffset) - v.pos.y * cos(angOffset);
				v.normal = ew::Normalize(v.pos);
				break;
			}
			verticiesList->push_back(v);
		}
	}
}