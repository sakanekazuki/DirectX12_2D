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
* 初期化
*/
bool PhysicsManager::Initialize()
{
	if (!physicsManager)
	{
		physicsManager = new PhysicsManager;
		if (!physicsManager)
		{
			Utility::DebugOutputFormatString("PhysicsManagerの生成に失敗しました");

			return false;
		}
	}
	return true;
}

/**
* 終了
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
* インスタンス取得
*/
PhysicsManager& PhysicsManager::Instance()
{
	return *physicsManager;
}

} // namespace Physics