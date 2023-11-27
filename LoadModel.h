#pragma once
#include <string>
#include <stdint.h>
#include <vector>
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

class LoadModel
{
public:
	struct VertexData
	{
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
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

	struct ModelData {
		//std::vector<VertexData> vertices;
		//MaterialData material;
		//std::vector<MeshData> meshs;
		MeshData meshs;
		size_t vertexNum;
	};
	static ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);
	static MaterialData LoadMaterialTemplateFile(const  std::string& directoryPath, const std::string& filename);
};

