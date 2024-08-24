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
* ������
*/
bool ShaderManager::Initialize()
{
	if (!shaderManager)
	{
		shaderManager = new ShaderManager;
		if (!shaderManager)
		{
			Utility::DebugOutputFormatString("�V�F�[�_�[�}�l�[�W���[�̐����Ɏ��s");
			return false;
		}
	}

	return true;
}

/**
* �I��
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
* �C���X�^���X�擾
*/
ShaderManager& ShaderManager::Instance()
{
	return *shaderManager;
}

/**
* �R���X�g���N�^
*/
ShaderManager::ShaderManager()
{
	// �C���X�^���X�擾
	File::FileManager& fileManager = File::FileManager::Instance();
	// �\�[�X�t�@�C���擾
	shaderFiles = fileManager.SeartchInExtension(".hlsl");
}

} // namespace ShaderManagement