#pragma once
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Matrix.h"
#include "Math/MatrixFunction.h"
#include "Math/Quaternion.h"

#include <stdint.h>
#include <string>
#include <vector>
#include <optional>
#include <map>
#include <span>
#include <array>

#include <wrl.h>
#include <d3d12.h>

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

//skybox用positionのみデータ
struct VertexDataSkyBox {
	Vector4 position;
};

struct MaterialData
{
	std::string textureFilePath;
	uint32_t textureHandle;
};

struct VertexWeightData {
	float weight;
	uint32_t vertexIndex;

};

struct JointWeightData {
	Matrix4x4 inverseBindPoseMatrix;
	std::vector<VertexWeightData> vertexWeights;
};

struct MeshData
{
	std::map<std::string, JointWeightData> skinClusterData;
	std::vector<VertexData> vertices;
	std::vector<uint32_t> indices;
	MaterialData material;
};

struct Index {
	uint32_t vertex;
	uint32_t uv;
	uint32_t normal;
};

struct MeshDataTerrain
{
	std::map<std::string, JointWeightData> skinClusterData;
	std::vector<VertexData> vertices;
	std::vector<Index> indices;
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

template<typename tValue>
struct Keyframe {
	float time;
	tValue value;
};

using KeyframeVector3 = Keyframe<Vector3>;
using KeyframeQuaternion = Keyframe<Quaternion>;

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

const uint32_t kNumMaxInfluence = 4;
struct VertexInfluence {
	std::array<float, kNumMaxInfluence> weight;
	std::array<int32_t, kNumMaxInfluence> jointIndices;
};

struct WellForGPU {
	Matrix4x4 skeletonSpaceMatrix;//位置
	Matrix4x4 skeletonSpaceInverseTransposeMatrix;//法線
};

struct SkinCluster {
	std::vector<Matrix4x4> inverseBindPoseMatrices;
	Microsoft::WRL::ComPtr<ID3D12Resource> influenceResource;
	D3D12_VERTEX_BUFFER_VIEW influenceBufferView;
	std::span<VertexInfluence> mappedInfluence;

	Microsoft::WRL::ComPtr<ID3D12Resource> paletteResource;
	std::span <WellForGPU> mappedPalette;
	std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> palleteSrvHandle;
};

struct MaterialParamater{
	Vector4 color;
	Vector4 disolveColor;
	int32_t enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
	float shininess;
	float growStrength;
	float environmentCoefficient;
	float disolveThreshold;
	int32_t shadingType;
};

struct OutLineData {
	Vector4 color;
	Matrix4x4 scale;
};
