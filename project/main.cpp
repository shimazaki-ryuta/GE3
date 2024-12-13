//メモリリークチェック
#include "D3DResourceLeakChacker.h"
//LeackChecker leackChecker;

#include <Windows.h>
#pragma comment(lib,"winmm.lib")
#undef min
#undef max
#include "Engine/GameManager.h"
// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	//システムタイマーの分解能を上げる
	timeBeginPeriod(1);

	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 0), &wsaData);
	HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
	assert(SUCCEEDED(hr));

	GameManager* game = new GameManager();
	game->Initialize();
	///メインループ
	game->Run();

	WSACleanup();
	return 0;
}