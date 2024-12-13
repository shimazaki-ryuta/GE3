#pragma once

#include <Windows.h>

//window関係
#include "CommonFiles/WinApp.h"
//directx12関係
#include "CommonFiles/DirectXCommon.h"
#include "Scene/IScene.h"
//#include "../GameScene.h"

//ゲームループ管理クラス

class GameManager
{
public:

	GameManager() {};
	~GameManager() {
		delete dxCommon;
#ifdef _DEBUG
		//デバッグカメラの解放
		debugController->Release();

#endif // _DEBUG
		//ウィンドウ解放
		CloseWindow(mainWindow->GetHwnd());

		CoUninitialize();
	};

	//初期化
	void Initialize();

	//メインループ
	void Run();

private:
	
	Window* mainWindow = nullptr;
	DirectXCommon* dxCommon = nullptr;
	IScene* scene = nullptr;
	ID3D12Debug1* debugController = nullptr;
};

