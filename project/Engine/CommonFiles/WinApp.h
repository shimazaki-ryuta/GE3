#pragma once
#include <Windows.h>
#include <cstdint>
#include <string>

class Window
{
public:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	/// <summary>
	/// ゲームウィンドウの作成
	/// </summary>
	void CreateGameWindow(const std::string& title,int32_t kClientWidth,int32_t kClientHight);
	
	/// <summary>
	/// メッセージの処理
	/// </summary>
	/// <returns>終了かどうか</returns>
	bool ProcessMessage();

	/// <summary>
	/// ウィンドウハンドルの取得
	/// </summary>
	inline HWND GetHwnd() { return hwnd_; };

	inline int32_t GetClientWidth() { return kClientWidth_; };
	inline int32_t GetClientHeight() { return kClientHeight_; };

	int32_t kClientWidth_, kClientHeight_;

private:
	WNDCLASS wc_{};
	HWND hwnd_;
	//int32_t kClientWidth_,kClientHeight_;

};