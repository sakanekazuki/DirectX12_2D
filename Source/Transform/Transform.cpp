/**
* @file Transform.cpp
*/

#include "Transform.h"

using namespace Math;

/**
* コンストラクタ
*/
Transform::Transform()
	:location(Vector3())
	,rotation(Vector4())
	,eulerAngle(Vector3())
	,scale(Vector3(1,1,1))
{}