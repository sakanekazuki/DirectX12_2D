/**
* @file Transform.h
*/

#ifndef TRANSFORM_H_INCLUDE
#define TRANSFORM_H_INCLUDE

#include "../Math/Vector3.h"
#include "../Math/Vector4.h"

class Transform
{
public:
	Transform();
	~Transform() = default;

private:
	// �ʒu
	Math::Vector3 location;
	// ��]
	Math::Vector4 rotation;

	Math::Vector3 eulerAngle;
	// �T�C�Y
	Math::Vector3 scale;

};

#endif // TRANSFORM_H_INCLUDE