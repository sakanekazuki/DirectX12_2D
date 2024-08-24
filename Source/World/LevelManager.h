/**
* @file LevelManager.h
*/

#ifndef LEVELMANAGER_H_INCLUDE
#define LEVELMANAGER_H_INCLUDE

#include <memory>

#include "Level.h"

namespace LevelManagement
{
// マップ管理クラス
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
	// コピー禁止
	LevelManager(const LevelManager&) = delete;
	LevelManager& operator=(const LevelManager&) = delete;

	// マップファイルの拡張子
	const char* fileExtension = ".level";

	// マップ情報を保存しているファイル
	std::vector<std::string> levelFiles;

	// 有効化されているマップ
	std::shared_ptr<Level> currentLevel;

};

} // namespace LevelManagement

#endif // LEVELMANAGER_H_INCLUDE