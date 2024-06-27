//メモリリークチェック
#include "D3DResourceLeakChacker.h"
//LeackChecker leackChecker;

#include <Windows.h>
#include "Engine/GameManager.h"
// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 0), &wsaData);
	HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
	assert(SUCCEEDED(hr));

	GameManager* game = new GameManager();
	game->Initialize();
	///メインループ
	game->Run();


	return 0;
}