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
* ������
*/
bool TextureManager::Initialize()
{
	if (!textureManager)
	{
		textureManager = new TextureManager;
		if (!textureManager)
		{
			Utility::DebugOutputFormatString("�e�N�X�`���}�l�[�W���̍쐬�Ɏ��s");
			return false;
		}
	}
	return true;
}

/**
* �I��
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
* �C���X�^���X�擾
*/
TextureManager& TextureManager::Instance()
{
	return *textureManager;
}

} // namespace TextureManagement