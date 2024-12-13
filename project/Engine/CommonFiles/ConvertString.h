#pragma once
#include <windows.h>
#include <string>

//マルチバイト文字列と相互変換

std::wstring ConvertString(const std::string& str);

std::string ConvertString(const std::wstring& str);
void Log(const std::string& message);