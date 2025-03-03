#pragma once

#include <Windows.h>

//window関係
#include "CommonFiles/WinApp.h"
//directx12関係
#include "CommonFiles/DirectXCommon.h"
#include "Scene/IScene.h"
//#include "../GameScene.h"
#include "../Application/Scene/SceneFactory.h"
//ゲームループ管理クラス

class GameManager
{
public:

	GameManager() {};
	~GameManager() {
		//delete dxCommon_;
#ifdef _DEBUG
		//デバッグカメラの解放
		debugController_->Release();

#endif // _DEBUG
		//ウィンドウ解放
		CloseWindow(mainWindow_->GetHwnd());

		CoUninitialize();
	};

	//初期化
	void Initialize();

	//メインループ
	void Run();

private:
	
	std::unique_ptr<Window> mainWindow_ = nullptr;
	std::unique_ptr<DirectXCommon> dxCommon_ = nullptr;
	std::unique_ptr<IScene> scene_;
	std::unique_ptr<SceneFactory> sceneFactory_;
	ID3D12Debug1* debugController_ = nullptr;
};

