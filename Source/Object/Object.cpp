/**
* @file Object.cpp
*/

#include "Object.h"

#include "../Utility/Utility.h"

/**
* �R���X�g���N�^
*/
Object::Object(std::string name, std::vector<std::string> tags)
	:name(name)
	,tags(tags)
{}

/**
* �f�X�g���N�^
*/
Object::~Object()
{}

/*
* 
*/
void Object::BeginPlay()
{

}

/**
* �X�V
* 
* @param deltaTime �X�V����
*/
void Object::Tick(float deltaTime)
{

}

/**
* �Փ�
*/
void Object::Hit()
{}

/**
* �d�Ȃ�
*/
void Object::BeginOverlap()
{}

/**
* ���O�擾
*/
std::string Object::GetName()
{
	return name;
}

/**
* �^�O�ǉ�
* 
* @param tag �ǉ�����^�O
*/
void Object::AddTag(const std::string& tag)
{
	if (!ActorHasTag(tag))
	{
		tags.push_back(tag);
	}
	else
	{
		Utility::DebugOutputFormatString("�^�O�͂��łɒǉ�����Ă��܂�");
	}
}

/**
* �^�O���������Ă��邩���ׂ�
* 
* @arapm tag ���ׂ�^�O
*/
bool Object::ActorHasTag(const std::string& tag)
{
	for (auto& str : tags)
	{
		if (str == tag)
		{
			return true;
		}
	}
	return false;
}