#pragma once

#include <Windows.h>

//window関係
#include "CommonFiles/WinApp.h"
//directx12関係
#include "CommonFiles/DirectXCommon.h"
#include "Scene/IScene.h"
//#include "../GameScene.h"

class GameManager
{
public:

	GameManager() {};
	~GameManager() {
		delete dxCommon;
#ifdef _DEBUG
		debugController->Release();

#endif // _DEBUG
		CloseWindow(mainWindow->GetHwnd());

		CoUninitialize();
	};

	void Initialize();
	void Run();

private:
	
	Window* mainWindow = nullptr;
	DirectXCommon* dxCommon = nullptr;
	IScene* scene = nullptr;
	ID3D12Debug1* debugController = nullptr;
};

