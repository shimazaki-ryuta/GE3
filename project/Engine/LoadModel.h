#pragma once
#include <string>
#include <stdint.h>
#include <vector>
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

#include "ModelStruct.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class LoadModel
{
public:
	
	static ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);
	static ModelData LoadModelFile(const std::string& directoryPath, const std::string& filename);
	static MaterialData LoadMaterialTemplateFile(const  std::string& directoryPath, const std::string& filename);
	static Node ReadNode(aiNode* node);
};

