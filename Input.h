#pragma once
#define DIRECTINPUT_VERSION	0x0800
#include <dinput.h>
#include <stdint.h>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

#include <wrl.h>

class Input
{
public:
	static Input* GetInstance();
	void Initialize(HWND hwnd);
	void KeyboardUpdate();

	static bool GetKey(uint8_t);
	static bool GetKeyDown(uint8_t);
	static bool GetKeyUp(uint8_t);

private:
	Input() = default;
	~Input() = default;
	Input(const Input&) = delete;
	Input operator=(const Input&) = delete;

	Microsoft::WRL::ComPtr<IDirectInput8> directInput_ = nullptr;

	Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboard_ = nullptr;
	BYTE key_[256] = {};
	BYTE preKey_[256] = {};

};

