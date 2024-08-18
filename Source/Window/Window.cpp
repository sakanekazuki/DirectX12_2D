/**
* @file Window.cpp
*/

#include "Window.h"

#include <tchar.h>

#include "../Utility/Utility.h"

namespace
{
WindowManagement::WindowManager* windowManager = nullptr;

LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// ウィンドウが破棄されたら呼ばれる
	if (msg == WM_DESTROY)
	{
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}
}

namespace WindowManagement
{
/**
* 初期化
*/
bool WindowManager::Initialize()
{
	if (!windowManager)
	{
		windowManager = new WindowManager;
		if (!windowManager)
		{
			return false;
		}
	}
	else
	{
		Utility::DebugOutputFormatString("WindowManager : 初期化済み");
	}

	return true;
}

/**
* 終了処理
*/
void WindowManager::Finalize()
{
	if (windowManager)
	{
		Utility::DebugOutputFormatString("WindowManager : 終了");
		delete windowManager;
		windowManager = nullptr;
	}
	else
	{
		Utility::DebugOutputFormatString("WindowManager : 初期化されていません");
	}
}

/**
* ウィンドウ管理クラス取得
*/
WindowManager& WindowManager::Instance()
{
	return *windowManager;
}

/**
* ウィンドウ生成
*/
HWND& WindowManager::CraeteWindow(
	const unsigned int width,
	const unsigned int height,
	std::string titleName)
{
	// ウィンドウが作成済みか調べる
	if (windowClasses.find(titleName) != windowClasses.end())
	{
		Utility::DebugOutputFormatString("ウィンドウは作成済みです");
		// 作成済みのウィンドウを返す
		return hwnds[titleName];
	}

	HINSTANCE hInstance = GetModuleHandle(nullptr);

	WNDCLASSEX windowClass = {};
	// ウィンドウクラス生成・登録
	windowClass.cbSize = sizeof(WNDCLASSEX);
	// コールバック関数の指定
	windowClass.lpfnWndProc = (WNDPROC)WindowProcedure;
	// アプリケーションクラス名
	windowClass.lpszClassName = _T("DirectX");
	// ハンドルの取得
	windowClass.hInstance = GetModuleHandle(0);
	// アプリケーションクラス
	RegisterClassEx(&windowClass);

	// ウィンドウサイズを決める
	RECT wrc = { 0, 0, width, height };
	// ウィンドウサイズの補正
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);
	// ウィンドウオブジェクトの生成
	HWND hwnd = CreateWindow(
		windowClass.lpszClassName,
		_T("title"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wrc.right - wrc.left,
		wrc.bottom - wrc.top,
		nullptr,
		nullptr,
		windowClass.hInstance,
		nullptr);

	// クラスとウィンドウ追加
	windowClasses[titleName] = windowClass;
	hwnds[titleName] = hwnd;

	return hwnd;
}

/**
* ウィンドウ表示
* 
* @param windowName 表示するウィンドウの名前
*/
bool WindowManager::ShowWindowByName(std::string windowName)
{
	if (hwnds.find(windowName) == hwnds.end())
	{
		Utility::DebugOutputFormatString("ウィンドウが存在しません");

		return false;
	}

	// ウィンドウ表示
	ShowWindow(hwnds[windowName], SW_SHOW);

	return true;
}

/**
* ウィンドウクラス取得
* 
* @param windowName 取得するウィンドウの名前
*/
WNDCLASSEX& WindowManager::GetWindowClass(std::string windowName)
{
	if (windowClasses.find(windowName) != windowClasses.end())
	{
		return windowClasses[windowName];
	}

	Utility::DebugOutputFormatString("ウィンドウは生成されていません");
	WNDCLASSEX w = {};
	return w;
}

/**
* ウィンドウ取得
* 
* @param windowName 取得するウィンドウの名前
*/
HWND& WindowManager::GetWindow(std::string windowName)
{
	if (hwnds.find(windowName) != hwnds.end())
	{
		return hwnds[windowName];
	}

	Utility::DebugOutputFormatString("ウィンドウが作成されていません");
	HWND h = {};

	return h;
}

} // namespace WindowManagement