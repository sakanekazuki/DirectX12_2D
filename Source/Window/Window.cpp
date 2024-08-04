/**
* @file Window.cpp
*/

#include "Window.h"

#include <tchar.h>

namespace WindowManagement
{
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

/**
* �E�B���h�E����
*/
void CraeteWindow(
	HWND& hwnd,
	WNDCLASSEX& windowClass,
	const unsigned int width,
	const unsigned int height)
{
	HINSTANCE hInstance = GetModuleHandle(nullptr);

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
	RECT wrc = { 0, 0, width, height};
	// �E�B���h�E�T�C�Y�̕␳
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);
	// �E�B���h�E�I�u�W�F�N�g�̐���
	hwnd = CreateWindow(
		windowClass.lpszClassName,
		_T("�^�C�g���o�["),
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