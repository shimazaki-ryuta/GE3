#pragma once
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Matrix.h"
#include "Math/Quaternion.h"

#include <stdint.h>
#include <string>
#include <vector>
#include <optional>
#include <map>

struct EulerTransform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

struct QuaternionTransform {
	Vector3 scale;
	Quaternion rotate;
	Vector3 translate;
};

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
	QuaternionTransform transform;
	Matrix4x4 localMatrix;
	std::string name;
	std::vector<Node> children;
};

struct ModelData {
	MeshData meshs;
	size_t vertexNum;
	Node rootNode;
};

struct Joint {
	QuaternionTransform transform;
	Matrix4x4 localMatrix;
	Matrix4x4 skeltonSpaceMatrix;
	std::string name;
	std::vector<int32_t> children;
	int32_t index;
	std::optional<int32_t> parent;
};

struct SkeletonData {
	int32_t root;
	std::map<std::string, int32_t> jointMap;
	std::vector<Joint> joints;
};