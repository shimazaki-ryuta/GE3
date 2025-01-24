#include "Engine/GameManager.h"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	GameManager* game = new GameManager();
	game->Initialize();
	///メインループ
	game->Run();

	return 0;
}