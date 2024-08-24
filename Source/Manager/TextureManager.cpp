/**
* @file TextureManager.cpp
*/

#include "TextureManager.h"

#include "../Utility/Utility.h"

namespace
{
TextureManagement::TextureManager* textureManager = nullptr;
}

namespace TextureManagement
{
/**
* 初期化
*/
bool TextureManager::Initialize()
{
	if (!textureManager)
	{
		textureManager = new TextureManager;
		if (!textureManager)
		{
			Utility::DebugOutputFormatString("テクスチャマネージャの作成に失敗");
			return false;
		}
	}
	return true;
}

/**
* 終了
*/
void TextureManager::Finalize()
{
	if (textureManager)
	{
		delete textureManager;
		textureManager = nullptr;
	}
}

/**
* インスタンス取得
*/
TextureManager& TextureManager::Instance()
{
	return *textureManager;
}

} // namespace TextureManagement