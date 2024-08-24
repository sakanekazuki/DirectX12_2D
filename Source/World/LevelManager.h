/**
* @file LevelManager.h
*/

#ifndef LEVELMANAGER_H_INCLUDE
#define LEVELMANAGER_H_INCLUDE

#include <memory>

#include "Level.h"

namespace LevelManagement
{
// �}�b�v�Ǘ��N���X
class LevelManager
{
public:
	static bool Initialize();
	static void Finalize();
	static LevelManager& Instance();

	~LevelManager() = default;

	bool CreateLevel(const std::string& levelName);
	void OpenLevel(const std::string& levelName);

private:
	LevelManager();
	// �R�s�[�֎~
	LevelManager(const LevelManager&) = delete;
	LevelManager& operator=(const LevelManager&) = delete;

	// �}�b�v�t�@�C���̊g���q
	const char* fileExtension = ".level";

	// �}�b�v����ۑ����Ă���t�@�C��
	std::vector<std::string> levelFiles;

	// �L��������Ă���}�b�v
	std::shared_ptr<Level> currentLevel;

};

} // namespace LevelManagement

#endif // LEVELMANAGER_H_INCLUDE