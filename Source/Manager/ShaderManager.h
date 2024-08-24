/**
* @file ShaderManager.h
*/

#ifndef SHADERMANAGER_H_INCLUDE
#define SHADERMANAGER_H_INCLUDE

#include <vector>
#include <string>

namespace ShaderManagement
{
// �V�F�[�_�[�Ǘ��N���X
class ShaderManager
{
public:
	static bool Initialize();
	static void Finalize();
	static ShaderManager& Instance();

	~ShaderManager() = default;

private:
	ShaderManager();
	// �R�s�[�֎~
	ShaderManager(const ShaderManager&) = delete;
	ShaderManager& operator=(const ShaderManager&) = delete;

	// �V�F�[�_�[�t�@�C��
	std::vector<std::string> shaderFiles;

};

} // namespace ShaderManagement

#endif // SHADERMANAGER_H_INCLUDE