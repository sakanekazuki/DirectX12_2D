/**
* @file Actor.cpp
*/

#include "Actor.h"

#include "../Utility/Utility.h"

/**
* �R���X�g���N�^
*/
Actor::Actor(std::string name, std::vector<std::string> tags)
	:name(name)
	, tags(tags)
{}

/**
* 
*/
void Actor::BeginPlay()
{

}

/**
* �X�V
* 
* @param deltaTime
*/
void Actor::Tick(float deltaTime)
{

}

/**
* �Փ�
*/
void Actor::Hit()
{}

/**
* �d�Ȃ����ۂ̏���
*/
void Actor::BeginOverlap()
{

}

/**
* ���O�擾
*/
std::string Actor::GetName()
{
	return name;
}

/**
* �^�O�ǉ�
* 
* @param tag �ǉ�����^�O
*/
void Actor::AddTag(std::string tag)
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
* @param tag ���ׂ�^�O
*/
bool Actor::ActorHasTag(std::string tag)
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