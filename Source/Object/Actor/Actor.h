/**
* @file Actor.h
*/

#ifndef ACTOR_H_INCLUDE
#define ACTOR_H_INCLUDE

#include <string>
#include <vector>
#include <memory>

#include "../Object.h"

// アクタークラス
class Actor : public Object
{
public:
	Actor() = default;
	Actor(std::string name, std::vector<std::string> tags);
	virtual ~Actor() = default;	

private:

};

#endif // ACTOR_H_INCLUDE