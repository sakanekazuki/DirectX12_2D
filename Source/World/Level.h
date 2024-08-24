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
// �}�b�v
class Level
{
public:
	Level(const std::string& name = "");
	virtual ~Level() = default;

	std::string GetName();

private:
	// ���O
	std::string name = "";

	// �}�b�v���̃A�N�^�[
	std::vector<std::shared_ptr<Actor>> actors;

};

} // namespace LevelManagement

#endif // LEVEL_H_INCLUDE