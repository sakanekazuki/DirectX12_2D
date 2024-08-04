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
// �A�v���P�[�V�����̃C���X�^���X
Application* application = nullptr;
}

/**
* ������
*/
bool Application::Initialize()
{
	if (!application)
	{
		// �A�v���P�[�V�����쐬
		application = new Application;
		if (!application)
		{
			Utility::DebugOutputFormatString("�A�v���P�[�V���������Ɏ��s\n");

			return false;
		}

		Utility::DebugOutputFormatString("�A�v���P�[�V��������\n");
	}
	else
	{
		Utility::DebugOutputFormatString("�A�v���P�[�V�����͐�����\n");
	}

	return true;
}

/**
* �C���X�^���X�擾
*/
Application& Application::Instance()
{
	return *application;
}

/**
* �A�v���P�[�V�����̃��[�v
*/
void Application::Run()
{

}

/**
* �㏈��
*/
void Application::Terminate()
{

}