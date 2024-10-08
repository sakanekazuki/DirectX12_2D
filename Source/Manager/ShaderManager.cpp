/**
* @file ShaderManager.cpp
*/

#include "ShaderManager.h"

#include "../Utility/Utility.h"
#include "FileManager.h"

namespace
{
ShaderManagement::ShaderManager* shaderManager = nullptr;
}

namespace ShaderManagement
{
/**
* 初期化
*/
bool ShaderManager::Initialize()
{
	if (!shaderManager)
	{
		shaderManager = new ShaderManager;
		if (!shaderManager)
		{
			Utility::DebugOutputFormatString("シェーダーマネージャーの生成に失敗");
			return false;
		}
	}

	return true;
}

/**
* 終了
*/
void ShaderManager::Finalize()
{
	if (shaderManager)
	{
		delete shaderManager;
		shaderManager = nullptr;
	}
}

/**
* インスタンス取得
*/
ShaderManager& ShaderManager::Instance()
{
	return *shaderManager;
}

/**
* コンストラクタ
*/
ShaderManager::ShaderManager()
{
	// インスタンス取得
	File::FileManager& fileManager = File::FileManager::Instance();
	// ソースファイル取得
	shaderFiles = fileManager.SeartchInExtension(".hlsl");
}

} // namespace ShaderManagement