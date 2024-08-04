/**
* @file Utility.cpp
*/

#include "Utility.h"

#include <Windows.h>
#include <iostream>

namespace Utility
{
/**
* �R���\�[����ʂɃt�H�[�}�b�g�t���������\��
*
* @param format �t�H�[�}�b�g
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