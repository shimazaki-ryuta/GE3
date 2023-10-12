#include "Input.h"
#include <assert.h>
#include <algorithm>
#include <cmath>
Input* Input::GetInstance()
{
	static Input instance;
	return &instance;
}


void Input::Initialize(HWND hwnd)
{
	HRESULT hr = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput_, nullptr);
	assert(SUCCEEDED(hr));

	//キーボードデバイス作成
	hr = directInput_->CreateDevice(GUID_SysKeyboard,&keyboard_, NULL);
	assert(SUCCEEDED(hr));

	//入力データ形式セット
	hr = keyboard_->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(hr));

	//排他制御レベルのセット
	hr = keyboard_->SetCooperativeLevel(hwnd,DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(hr));
	/*
	//デバイスの列挙
	//LPDIENUMDEVICESCALLBACK DeviceFindCallBack;
	LPVOID parameta;
	hr = directInput_->EnumDevices(DI8DEVTYPE_JOYSTICK,DeviceFindCallBack,&parameta,DIEDFL_ATTACHEDONLY);
	assert(SUCCEEDED(hr));
	*/


}

bool Input::GetJoystickState(uint8_t stickNo, XINPUT_STATE& out)
{
	DWORD result = XInputGetState(stickNo, &out);
	//out.Gamepad.sThumbLX = std::max();
	if (unsigned(std::abs(out.Gamepad.sThumbLX)) <= Input::GetInstance()->deadzoneLeft)
	{
		out.Gamepad.sThumbLX = 0;
	}
	if (unsigned(std::abs(out.Gamepad.sThumbLY)) <= Input::GetInstance()->deadzoneLeft)
	{
		out.Gamepad.sThumbLY = 0;
	}
	if (unsigned(std::abs(out.Gamepad.sThumbRX)) <= Input::GetInstance()->deadzoneRight)
	{
		out.Gamepad.sThumbRX = 0;
	}
	if (unsigned(std::abs(out.Gamepad.sThumbRY)) <= Input::GetInstance()->deadzoneRight)
	{
		out.Gamepad.sThumbRY = 0;
	}
	if (result == ERROR_SUCCESS)
	{
		return true;
	}
	return false;
}


void Input::KeyboardUpdate()
{
	memcpy(preKey_, key_, 256);
	keyboard_->Acquire();
	keyboard_->GetDeviceState(sizeof(key_),key_);

}

bool Input::GetKey(uint8_t keyCode)
{
	return GetInstance()->key_[keyCode];
}

bool Input::GetKeyDown(uint8_t keyCode)
{
	return GetInstance()->key_[keyCode] && !GetInstance()->preKey_[keyCode];
}

bool Input::GetKeyUp(uint8_t keyCode)
{
	return !GetInstance()->key_[keyCode] && GetInstance()->preKey_[keyCode];
}