/**
* @file Object.h
*/

#ifndef OBJECT_H_INCLUDE
#define OBJECT_H_INCLUDE

#include <vector>
#include <string>
#include <memory>

#include "../Component/Component.h"

// �I�u�W�F�N�g
class Object
{
public:
	Object(std::string name, std::vector<std::string> tags);
	virtual ~Object();

	std::string GetName();

	void AddTag(const std::string& tag);
	bool ActorHasTag(const std::string& tag);

	virtual void BeginPlay();
	virtual void Tick(float deltaTime);

	virtual void Hit();
	virtual void BeginOverlap();

private:
	// ���O
	std::string name = "";
	// �^�O
	std::vector<std::string> tags;

	// �R���|�[�l���g
	std::vector<std::shared_ptr<Component>> components;

protected:

};

#endif // OBJECT_H_INCLUDE