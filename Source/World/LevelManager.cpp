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
* ������
*/
bool LevelManager::Initialize()
{
	if (!levelManager)
	{
		levelManager = new LevelManager;
		if (!levelManager)
		{
			Utility::DebugOutputFormatString("���x���}�l�[�W���[�̍쐬�Ɏ��s");
			return false;
		}
	}
	return true;
}

/**
* �I��
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
* �C���X�^���X�擾
*/
LevelManager& LevelManager::Instance()
{
	return *levelManager;
}

/**
* �R���X�g���N�^
*/
LevelManager::LevelManager()
{
	// �t�@�C���Ǘ��N���X�擾
	FileManagement::FileManager& fileManager = FileManagement::FileManager::Instance();
	// �t�@�C���擾
	levelFiles = fileManager.SeartchInExtension(fileExtension);
}

/**
* �}�b�v�쐻
* 
* @param levelName �}�b�v��
*/
bool LevelManager::CreateLevel(const std::string& levelName)
{


	return true;
}

/**
* �}�b�v���J��
* 
* @param levelName �}�b�v��
*/
void LevelManager::OpenLevel(const std::string& levelName)
{
	// �L���̃}�b�v���Z�b�g
	currentLevel.reset();
}

} // namespace LevelManagement