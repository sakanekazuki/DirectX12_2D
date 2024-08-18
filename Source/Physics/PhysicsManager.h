/**
* @file PhysicsManager.h
*/

#ifndef PHYSICSMANAGER_H_INCLUDE
#define PHYSICSMANAGER_H_INCLUDE

namespace Physics
{
// �����Ǘ��N���X
class PhysicsManager
{
public:
	static bool Initialize();
	static void Finalize();
	static PhysicsManager& Instance();

private:
	PhysicsManager() = default;
	PhysicsManager(const PhysicsManager&) = delete;
	~PhysicsManager() = default;

	PhysicsManager& operator=(const PhysicsManager&) = delete;

};

} // namespace Physics

#endif // PHYSICSMANAGER_H_INCLUDE