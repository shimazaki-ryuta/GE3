#include "SceneFactory.h"
#include "GameScene.h"
IScene* SceneFactory::CreateScene(const std::string& sceneName) {
	IScene* scene = nullptr;
	if (sceneName == "DEFAULT" || sceneName.empty()) {
		scene = new GameScene;
	}

	return scene;
}