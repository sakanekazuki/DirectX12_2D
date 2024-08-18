/**
* @file PhysicsManager.cpp
*/

#include "PhysicsManager.h"

#include "../Utility/Utility.h"

namespace
{
Physics::PhysicsManager* physicsManager = nullptr;
}

namespace Physics
{
/**
* ������
*/
bool PhysicsManager::Initialize()
{
	if (!physicsManager)
	{
		physicsManager = new PhysicsManager;
		if (!physicsManager)
		{
			Utility::DebugOutputFormatString("PhysicsManager�̐����Ɏ��s���܂���");

			return false;
		}
	}
	return true;
}

/**
* �I��
*/
void PhysicsManager::Finalize()
{
	if (physicsManager)
	{
		delete physicsManager;
		physicsManager = nullptr;
	}
}

/**
* �C���X�^���X�擾
*/
PhysicsManager& PhysicsManager::Instance()
{
	return *physicsManager;
}

} // namespace Physics