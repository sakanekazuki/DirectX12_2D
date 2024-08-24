/**
* @file Level.h
*/

#ifndef LEVEL_H_INCLUDE
#define LEVEL_H_INCLUDE

#include <string>
#include <vector>
#include <memory>

#include "../Actor/Actor.h"

namespace LevelManagement
{
// マップ
class Level
{
public:
	Level(const std::string& name = "");
	virtual ~Level() = default;

	std::string GetName();

private:
	// 名前
	std::string name = "";

	// マップ内のアクター
	std::vector<std::shared_ptr<Actor>> actors;

};

} // namespace LevelManagement

#endif // LEVEL_H_INCLUDE