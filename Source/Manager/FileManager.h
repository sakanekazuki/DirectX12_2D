/**
* @file FileMnaager.h
*/

#ifndef FILEMANAGER_H_INCLUDE
#define FILEMANAGER_H_INCLUDE

#include <vector>
#include <string>
#include <filesystem>

namespace FileManagement
{
// ファイル管理クラス
class FileManager final
{
public:
	~FileManager() = default;

	static bool Initialize();
	static void Finalize();
	static FileManager& Instance();

	std::vector<std::string> GetSourceFiles();

	std::vector<std::string> AllFileSeartchFromDirectory(const std::string& directoryName);

	std::vector<std::string> SeartchInExtension(const std::string& extension);

private:
	FileManager();
	// コピーコンストラクタとコピーの禁止
	FileManager(const FileManager&) = delete;
	FileManager& operator=(const FileManager&) = delete;

	// プロジェクトのパス
	std::string projectPath;

	// ファイル名
	std::vector<std::string> resourceFileNames;
	std::vector<std::string> sourceFileNames;

};

} // namespace FileManagement

#endif // FILEMANAGER_H_INCLUDE