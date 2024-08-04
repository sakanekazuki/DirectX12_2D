/**
* @file Application.cpp
*/

#include "Application.h"

#include <iostream>
#include <memory>

#include "Utility/Utility.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "DirectXTex.lib")

namespace
{
// アプリケーションのインスタンス
Application* application = nullptr;
}

/**
* 初期化
*/
bool Application::Initialize()
{
	if (!application)
	{
		// アプリケーション作成
		application = new Application;
		if (!application)
		{
			Utility::DebugOutputFormatString("アプリケーション生成に失敗\n");

			return false;
		}

		Utility::DebugOutputFormatString("アプリケーション生成\n");
	}
	else
	{
		Utility::DebugOutputFormatString("アプリケーションは生成済\n");
	}

	return true;
}

/**
* インスタンス取得
*/
Application& Application::Instance()
{
	return *application;
}

/**
* アプリケーションのループ
*/
void Application::Run()
{

}

/**
* 後処理
*/
void Application::Terminate()
{

}