/**
* @file Actor.cpp
*/

#include "Actor.h"

#include "../Utility/Utility.h"

/**
* コンストラクタ
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
* 更新
* 
* @param deltaTime
*/
void Actor::Tick(float deltaTime)
{

}

/**
* 衝突
*/
void Actor::Hit()
{}

/**
* 重なった際の処理
*/
void Actor::BeginOverlap()
{

}

/**
* 名前取得
*/
std::string Actor::GetName()
{
	return name;
}

/**
* タグ追加
* 
* @param tag 追加するタグ
*/
void Actor::AddTag(std::string tag)
{
	if (!ActorHasTag(tag))
	{
		tags.push_back(tag);
	}
	else
	{
		Utility::DebugOutputFormatString("タグはすでに追加されています");
	}
}

/**
* タグを所持しているか調べる
* 
* @param tag 調べるタグ
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