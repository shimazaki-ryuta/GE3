#pragma once
#include "../../Engine/Scene/AbstractSceneFactory.h"

//シーンファクトリー

class SceneFactory : AbstractSceneFactory
{
public:
	//シーン生成
	IScene* CreateScene(const std::string& sceneName = "");
private:

};
