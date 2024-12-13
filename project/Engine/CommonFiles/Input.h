#pragma once
#define DIRECTINPUT_VERSION	0x0800
#include <dinput.h>
#include <stdint.h>
#include <Windows.h>
#include <Xinput.h>
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"xinput.lib")
#include <wrl.h>

//入力クラス(キーボード、コントローラー)

class Input
{
public:
	//インスタンス取得
	static Input* GetInstance();
	//初期化
	void Initialize(HWND hwnd);
	//キーボード情報更新
	void KeyboardUpdate();

	//指定キー入力の有無
	static bool GetKey(uint8_t);
	static bool GetKeyDown(uint8_t);
	static bool GetKeyUp(uint8_t);

	//コントローラーの入力情報を得る
	static bool GetJoystickState(uint8_t stickNo, XINPUT_STATE& out);


private:
	Input() = default;
	~Input() = default;
	Input(const Input&) = delete;
	Input operator=(const Input&) = delete;

	Microsoft::WRL::ComPtr<IDirectInput8> directInput_ = nullptr;

	Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboard_ = nullptr;

	//Microsoft::WRL::ComPtr<IDirectInputDevice8> joyStick_ = nullptr;

	uint32_t deadzoneLeft = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
	uint32_t deadzoneRight = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;


	BYTE key_[256] = {};
	BYTE preKey_[256] = {};

};

