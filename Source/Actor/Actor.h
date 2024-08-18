/**
* @file Actor.h
*/

#ifndef ACTOR_H_INCLUDE
#define ACTOR_H_INCLUDE

#include <string>
#include <vector>
#include <memory>

#include "../Component/Component.h"

// �A�N�^�[�N���X
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
	// ���O
	std::string name;
	// �^�O
	std::vector<std::string> tags;

	// �R���|�[�l���g
	std::vector<std::shared_ptr<Component>> components;

};

#endif // ACTOR_H_INCLUDE