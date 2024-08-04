/**
* @file Utility.cpp
*/

#include "Utility.h"

#include <Windows.h>
#include <iostream>

namespace Utility
{
/**
* コンソール画面にフォーマット付き文字列を表示
*
* @param format フォーマット
*/
void DebugOutputFormatString(const char* format, ...)
{
#ifdef _DEBUG
	va_list valist;
	va_start(valist, format);
	printf_s(format, valist);
	va_end(valist);
#endif
}

} // namespace Utility