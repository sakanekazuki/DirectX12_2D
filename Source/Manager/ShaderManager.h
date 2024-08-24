/**
* @file ShaderManager.h
*/

#ifndef SHADERMANAGER_H_INCLUDE
#define SHADERMANAGER_H_INCLUDE

#include <vector>
#include <string>

namespace ShaderManagement
{
// シェーダー管理クラス
class ShaderManager
{
public:
	static bool Initialize();
	static void Finalize();
	static ShaderManager& Instance();

	~ShaderManager() = default;

private:
	ShaderManager();
	// コピー禁止
	ShaderManager(const ShaderManager&) = delete;
	ShaderManager& operator=(const ShaderManager&) = delete;

	// シェーダーファイル
	std::vector<std::string> shaderFiles;

};

} // namespace ShaderManagement

#endif // SHADERMANAGER_H_INCLUDE