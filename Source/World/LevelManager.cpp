/**
* @file LevelManager.cpp
*/

#include "LevelManager.h"
#include "../Manager/FileManager.h"

#include "../Utility/Utility.h"

namespace
{
LevelManagement::LevelManager* levelManager = nullptr;
}

namespace LevelManagement
{
/**
* 初期化
*/
bool LevelManager::Initialize()
{
	if (!levelManager)
	{
		levelManager = new LevelManager;
		if (!levelManager)
		{
			Utility::DebugOutputFormatString("レベルマネージャーの作成に失敗");
			return false;
		}
	}
	return true;
}

/**
* 終了
*/
void LevelManager::Finalize()
{
	if (levelManager)
	{
		delete levelManager;
		levelManager = nullptr;
	}
}

/**
* インスタンス取得
*/
LevelManager& LevelManager::Instance()
{
	return *levelManager;
}

/**
* コンストラクタ
*/
LevelManager::LevelManager()
{
	// ファイル管理クラス取得
	FileManagement::FileManager& fileManager = FileManagement::FileManager::Instance();
	// ファイル取得
	levelFiles = fileManager.SeartchInExtension(fileExtension);
}

/**
* マップ作製
* 
* @param levelName マップ名
*/
bool LevelManager::CreateLevel(const std::string& levelName)
{


	return true;
}

/**
* マップを開く
* 
* @param levelName マップ名
*/
void LevelManager::OpenLevel(const std::string& levelName)
{
	// 有効のマップリセット
	currentLevel.reset();
}

} // namespace LevelManagement