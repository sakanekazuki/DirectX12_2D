/**
* @file String.cpp
*/

#include "String.h"

#include <Windows.h>
//#include <stringapiset.h>

namespace STL
{
/**
* コンストラクタ
*
* @param str 文字列
*/
String::String(const char* str)
{
	// サイズ設定
	size = strlen(str);
	// 初期化
	data = new char[size + 1];
	strcpy_s(data, size + 1, str);
}

/**
* デストラクタ
*/
String::~String()
{
	delete[] data;
	data = nullptr;
}

String::operator const char* ()
{
	return data;
}

String::operator const std::string()
{
	return std::string(data);
}

String::operator const std::wstring()
{
	// 変換後の文字数を取得
	int outsize = MultiByteToWideChar(CP_ACP, 0, data, size, 0, 0);
	std::wstring result(outsize, L'\0');
	// 文字数が0ではない場合変換する
	if (outsize)
	{
		MultiByteToWideChar(CP_ACP, 0, data, size, &result[0], outsize);
	}
	return result;
}

String::operator const std::u8string()
{
	std::string str(data);
	std::u8string result(str.begin(), str.end());
	return result;
}

//const char* String::operator+(const char* c)
//{
//	return nullptr;
//}
//
//const char* String::operator+(const String& s)
//{
//	return nullptr;
//}

} // namespace EngineUtility