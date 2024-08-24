/**
* @file Level.cpp
*/

#include "Level.h"

namespace LevelManagement
{
/**
* コンストラクタ
*/
Level::Level(const std::string& name)
	:name(name)
{}

/**
* マップ名取得
*/
std::string Level::GetName()
{
	return name;
}

} // namespace LevelManagement