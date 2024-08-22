/**
* @file FileManager.cpp
*/

#include "FileManager.h"

#include <Windows.h>
#include <tchar.h>

#include "../Utility/Utility.h"

//#define BUFSIZE 1024

namespace
{
File::FileManager* fileManager = nullptr;
}

namespace File
{
/**
* ������
*/
bool FileManager::Initialize()
{
	if (!fileManager)
	{
		fileManager = new FileManager;
		if (!fileManager)
		{
			Utility::DebugOutputFormatString("�t�@�C���}�l�[�W���[�̍쐬�Ɏ��s���܂���");
			return false;
		}
	}
	return true;
}

/**
* �I��
*/
void FileManager::Finalize()
{
	if (fileManager)
	{
		delete fileManager;
		fileManager = nullptr;
	}
}

/**
* �t�@�C���}�l�[�W���[�擾
*/
FileManager& FileManager::Instance()
{
	return *fileManager;
}

/**
* �R���X�g���N�^
*/
FileManager::FileManager()
{
	TCHAR TDirectoryName[255];
	// ���݂̃f�B���N�g���擾
	GetCurrentDirectory(255, TDirectoryName);

	char directoryName[255];
	// TCHAR����char[]�ɕϊ�
	WideCharToMultiByte(CP_ACP, 0, TDirectoryName, -1, directoryName, 255, NULL, NULL);
	// ���݂̃f�B���N�g���ۑ�
	projectPath = directoryName;

	// �t�@�C�����ݒ�
	resourceFileNames = AllFileSeartchFromDirectory(projectPath + "/Resource");
	sourceFileNames = AllFileSeartchFromDirectory(projectPath + "/Source");
}

/**
* �\�[�X�t�H���_���̃t�@�C���擾
*/
std::vector<std::string> FileManager::GetSourceFiles()
{
	return sourceFileNames;
}

/**
* �f�B���N�g�����̑S�Ẵt�@�C���擾
* 
* @param directoryName ��������f�B���N�g��
* 
* @return ���݂���t�H���_�ƃf�B���N�g��
*/
std::vector<std::string> FileManager::AllFileSeartchFromDirectory(const std::string& directoryName)
{
	// ����
	std::vector<std::string> result;

	if (!std::filesystem::exists(directoryName))
	{
		Utility::DebugOutputFormatString("�f�B���N�g�������݂��܂���\n");
		return result;
	}
	// �f�B���N�g���̃C�e���[�^
	auto directoryIterator = std::filesystem::directory_iterator(directoryName);
	for (auto& it : directoryIterator)
	{
		if (it.is_directory())
		{
			// �f�B���N�g���̏ꍇ������x����
			std::vector<std::string> files = AllFileSeartchFromDirectory(it.path().string());
			// �S�Ẵt�@�C����ǉ�
			for (auto& file : files)
			{
				result.push_back(file);
			}
		}
		else
		{
			// ���ʂɒǉ�
			result.push_back(it.path().string());
		}
	}

	return result;
}

/**
* �t�@�C���̎�ނŒT��
* 
* @param fileType �t�@�C���̎��
*/
std::vector<std::string> FileManager::SeartchInExtension(const std::string& extension)
{
	// ����
	std::vector<std::string> result;
	// �\�[�X�t�@�C���̊g���q�𒲂ׂ�
	for (auto& file : sourceFileNames)
	{
		std::filesystem::path path(file);
		auto e = path.extension();
		if (extension == path.extension())
		{
			result.push_back(file);
		}
	}
	// ���\�[�X�t�@�C���̊g���q�𒲂ׂ�
	for (auto& file : resourceFileNames)
	{
		std::filesystem::path path(file);
		if (extension == path.extension())
		{
			result.push_back(file);
		}
	}

	return result;
}

} // namespace File