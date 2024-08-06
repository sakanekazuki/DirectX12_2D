/**
* @file String.h
*/

#ifndef STRING_H_INCLUDE
#define STRING_H_INCLUDE

#include <string>

namespace STL
{
// ������^
class String
{
public:
	String() = default;
	String(const char* str);
	~String();

	operator const char* ();
	operator const std::string();
	operator const std::wstring();
	operator const std::u8string();

	//const String operator=(const char* c);

	//const char* operator + (const char* c);
	//const char* operator + (const String& s);

private:
	// ������
	char* data = nullptr;
	// ������̒���
	size_t size = 0;

};
} // namespace STL

#endif // STRING_H_INCLUDE