#pragma once
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Matrix.h"

#include <stdint.h>
#include <string>
#include <vector>

struct VertexData
{
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
	Vector4 triangleCenter;
};
struct MaterialData
{
	std::string textureFilePath;
	uint32_t textureHandle;
};

struct MeshData
{
	std::vector<VertexData> vertices;
	MaterialData material;
};

struct Node {
	Matrix4x4 localMatrix;
	std::string name;
	std::vector<Node> children;
};

struct ModelData {
	MeshData meshs;
	size_t vertexNum;
	Node rootNode;
};