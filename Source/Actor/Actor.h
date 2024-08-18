/**
* @file Actor.h
*/

#ifndef ACTOR_H_INCLUDE
#define ACTOR_H_INCLUDE

#include <string>
#include <vector>
#include <memory>

#include "../Component/Component.h"

// アクタークラス
class Actor
{
public:
	Actor() = default;
	Actor(std::string name, std::vector<std::string> tags);
	virtual ~Actor() = default;

	virtual void BeginPlay();
	virtual void Tick(float deltaTime);

	virtual void Hit();
	virtual void BeginOverlap();

	std::string GetName();

	void AddTag(std::string tag);
	bool ActorHasTag(std::string tag);

private:
	// 名前
	std::string name;
	// タグ
	std::vector<std::string> tags;

	// コンポーネント
	std::vector<std::shared_ptr<Component>> components;

};

#endif // ACTOR_H_INCLUDE