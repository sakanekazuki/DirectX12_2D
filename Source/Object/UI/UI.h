/**
* @file UI.h
*/

#ifndef UI_H_INCLUDE
#define UI_H_INCLUDE

#include "../Object.h"

class UI : public Object
{
public:
	UI(std::string name,std::vector<std::string> tags);
	virtual ~UI();

	virtual void OnFocus();
	virtual void PointerOver();
	virtual void Click();

private:

};

#endif // UI_H_INCLUDE