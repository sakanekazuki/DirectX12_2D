/**
* @file Window.h
*/

#ifndef WINDOW_H_INCLUDE
#define WINDOW_H_INCLUDE

#include <Windows.h>

// �E�B���h�E�Ǘ�
namespace WindowManagement
{
void CraeteWindow(HWND& hwnd, WNDCLASSEX& windowClass, const unsigned int width, const unsigned int height);
} // namespace WindowManagement

#endif // WINDOW_H_INCLUDE