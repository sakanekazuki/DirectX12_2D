/**
* @file Level.cpp
*/

#include "Level.h"

namespace LevelManagement
{
/**
* �R���X�g���N�^
*/
Level::Level(const std::string& name)
	:name(name)
{}

/**
* �}�b�v���擾
*/
std::string Level::GetName()
{
	return name;
}

} // namespace LevelManagement