#pragma once
#include "IScene.h"
#include <string>
//シーンファクトリーの中小クラス
class AbstractSceneFactory
{
public:
	AbstractSceneFactory() {};
	virtual ~AbstractSceneFactory() {};

	//シーン生成
	virtual IScene* CreateScene(const std::string& sceneName = "") = 0;

private:

};
