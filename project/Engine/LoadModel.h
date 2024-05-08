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
	static SkeletonData CreateSkelton(const Node& rootNode);
	static int32_t CreateJoint(const Node& node,const std::optional<int32_t>& parent,std::vector<Joint>& joints);

	static Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time);
	static Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time);
};

