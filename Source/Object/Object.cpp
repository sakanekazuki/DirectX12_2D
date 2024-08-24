/**
* @file Object.cpp
*/

#include "Object.h"

#include "../Utility/Utility.h"

/**
* コンストラクタ
*/
Object::Object(std::string name, std::vector<std::string> tags)
	:name(name)
	,tags(tags)
{}

/**
* デストラクタ
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
* 更新
* 
* @param deltaTime 更新時間
*/
void Object::Tick(float deltaTime)
{

}

/**
* 衝突
*/
void Object::Hit()
{}

/**
* 重なり
*/
void Object::BeginOverlap()
{}

/**
* 名前取得
*/
std::string Object::GetName()
{
	return name;
}

/**
* タグ追加
* 
* @param tag 追加するタグ
*/
void Object::AddTag(const std::string& tag)
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
* @arapm tag 調べるタグ
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