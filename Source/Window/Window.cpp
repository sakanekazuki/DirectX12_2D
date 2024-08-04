/**
* @file Window.cpp
*/

#include "Window.h"

#include <tchar.h>

namespace WindowManagement
{
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

/**
* ウィンドウ生成
*/
void CraeteWindow(
	HWND& hwnd,
	WNDCLASSEX& windowClass,
	const unsigned int width,
	const unsigned int height)
{
	HINSTANCE hInstance = GetModuleHandle(nullptr);

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
	RECT wrc = { 0, 0, width, height};
	// ウィンドウサイズの補正
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);
	// ウィンドウオブジェクトの生成
	hwnd = CreateWindow(
		windowClass.lpszClassName,
		_T("タイトルバー"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wrc.right - wrc.left,
		wrc.bottom - wrc.top,
		nullptr,
		nullptr,
		windowClass.hInstance,
		nullptr);
}

} // namespace WindowManagement