/**
* @file TextureManager.h
*/

#ifndef TEXTUREMANAGER_H_INCLUDE
#define TEXTUREMANAGER_H_INCLUDE

namespace TextureManagement
{
// �e�N�X�`���Ǘ��N���X
class TextureManager
{
public:
	static bool Initialize();
	static void Finalize();
	static TextureManager& Instance();

	~TextureManager() = default;

private:

};

} // namespace TextureManagement

#endif // TEXTUREMANAGER_H_INCLUDE