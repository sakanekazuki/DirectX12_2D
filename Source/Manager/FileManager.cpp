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
* 初期化
*/
bool FileManager::Initialize()
{
	if (!fileManager)
	{
		fileManager = new FileManager;
		if (!fileManager)
		{
			Utility::DebugOutputFormatString("ファイルマネージャーの作成に失敗しました");
			return false;
		}
	}
	return true;
}

/**
* 終了
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
* ファイルマネージャー取得
*/
FileManager& FileManager::Instance()
{
	return *fileManager;
}

/**
* コンストラクタ
*/
FileManager::FileManager()
{
	TCHAR TDirectoryName[255];
	// 現在のディレクトリ取得
	GetCurrentDirectory(255, TDirectoryName);

	char directoryName[255];
	// TCHARからchar[]に変換
	WideCharToMultiByte(CP_ACP, 0, TDirectoryName, -1, directoryName, 255, NULL, NULL);
	// 現在のディレクトリ保存
	projectPath = directoryName;

	// ファイル名設定
	resourceFileNames = AllFileSeartchFromDirectory(projectPath + "/Resource");
	sourceFileNames = AllFileSeartchFromDirectory(projectPath + "/Source");
}

/**
* ソースフォルダ内のファイル取得
*/
std::vector<std::string> FileManager::GetSourceFiles()
{
	return sourceFileNames;
}

/**
* ディレクトリ内の全てのファイル取得
* 
* @param directoryName 検索するディレクトリ
* 
* @return 存在するフォルダとディレクトリ
*/
std::vector<std::string> FileManager::AllFileSeartchFromDirectory(const std::string& directoryName)
{
	// 結果
	std::vector<std::string> result;

	if (!std::filesystem::exists(directoryName))
	{
		Utility::DebugOutputFormatString("ディレクトリが存在しません\n");
		return result;
	}
	// ディレクトリのイテレータ
	auto directoryIterator = std::filesystem::directory_iterator(directoryName);
	for (auto& it : directoryIterator)
	{
		if (it.is_directory())
		{
			// ディレクトリの場合もう一度検索
			std::vector<std::string> files = AllFileSeartchFromDirectory(it.path().string());
			// 全てのファイルを追加
			for (auto& file : files)
			{
				result.push_back(file);
			}
		}
		else
		{
			// 結果に追加
			result.push_back(it.path().string());
		}
	}

	return result;
}

/**
* ファイルの種類で探す
* 
* @param fileType ファイルの種類
*/
std::vector<std::string> FileManager::SeartchInExtension(const std::string& extension)
{
	// 結果
	std::vector<std::string> result;
	// ソースファイルの拡張子を調べる
	for (auto& file : sourceFileNames)
	{
		std::filesystem::path path(file);
		auto e = path.extension();
		if (extension == path.extension())
		{
			result.push_back(file);
		}
	}
	// リソースファイルの拡張子を調べる
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