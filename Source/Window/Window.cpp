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
	// �E�B���h�E���j�����ꂽ��Ă΂��
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
* ������
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
		Utility::DebugOutputFormatString("WindowManager : �������ς�");
	}

	return true;
}

/**
* �I������
*/
void WindowManager::Finalize()
{
	if (windowManager)
	{
		Utility::DebugOutputFormatString("WindowManager : �I��");
		delete windowManager;
		windowManager = nullptr;
	}
	else
	{
		Utility::DebugOutputFormatString("WindowManager : ����������Ă��܂���");
	}
}

/**
* �E�B���h�E�Ǘ��N���X�擾
*/
WindowManager& WindowManager::Instance()
{
	return *windowManager;
}

/**
* �E�B���h�E����
*/
HWND& WindowManager::CraeteWindow(
	const unsigned int width,
	const unsigned int height,
	std::string titleName)
{
	// �E�B���h�E���쐬�ς݂����ׂ�
	if (windowClasses.find(titleName) != windowClasses.end())
	{
		Utility::DebugOutputFormatString("�E�B���h�E�͍쐬�ς݂ł�");
		// �쐬�ς݂̃E�B���h�E��Ԃ�
		return hwnds[titleName];
	}

	HINSTANCE hInstance = GetModuleHandle(nullptr);

	WNDCLASSEX windowClass = {};
	// �E�B���h�E�N���X�����E�o�^
	windowClass.cbSize = sizeof(WNDCLASSEX);
	// �R�[���o�b�N�֐��̎w��
	windowClass.lpfnWndProc = (WNDPROC)WindowProcedure;
	// �A�v���P�[�V�����N���X��
	windowClass.lpszClassName = _T("DirectX");
	// �n���h���̎擾
	windowClass.hInstance = GetModuleHandle(0);
	// �A�v���P�[�V�����N���X
	RegisterClassEx(&windowClass);

	// �E�B���h�E�T�C�Y�����߂�
	RECT wrc = { 0, 0, width, height };
	// �E�B���h�E�T�C�Y�̕␳
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);
	// �E�B���h�E�I�u�W�F�N�g�̐���
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

	// �N���X�ƃE�B���h�E�ǉ�
	windowClasses[titleName] = windowClass;
	hwnds[titleName] = hwnd;

	return hwnd;
}

/**
* �E�B���h�E�\��
* 
* @param windowName �\������E�B���h�E�̖��O
*/
bool WindowManager::ShowWindowByName(std::string windowName)
{
	if (hwnds.find(windowName) == hwnds.end())
	{
		Utility::DebugOutputFormatString("�E�B���h�E�����݂��܂���");

		return false;
	}

	// �E�B���h�E�\��
	ShowWindow(hwnds[windowName], SW_SHOW);

	return true;
}

/**
* �E�B���h�E�N���X�擾
* 
* @param windowName �擾����E�B���h�E�̖��O
*/
WNDCLASSEX& WindowManager::GetWindowClass(std::string windowName)
{
	if (windowClasses.find(windowName) != windowClasses.end())
	{
		return windowClasses[windowName];
	}

	Utility::DebugOutputFormatString("�E�B���h�E�͐�������Ă��܂���");
	WNDCLASSEX w = {};
	return w;
}

/**
* �E�B���h�E�擾
* 
* @param windowName �擾����E�B���h�E�̖��O
*/
HWND& WindowManager::GetWindow(std::string windowName)
{
	if (hwnds.find(windowName) != hwnds.end())
	{
		return hwnds[windowName];
	}

	Utility::DebugOutputFormatString("�E�B���h�E���쐬����Ă��܂���");
	HWND h = {};

	return h;
}

} // namespace WindowManagement