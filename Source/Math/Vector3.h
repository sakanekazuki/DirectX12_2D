/**
* @file Vector3.h
*/

#ifndef VECTOR3_H_INCLUDE
#define VECTOR3_H_INCLUDE

namespace Math
{
class Vector3
{
public:
	Vector3(float x = 0, float y = 0, float z = 0);
	~Vector3() = default;

	float x = 0;
	float y = 0;
	float z = 0;

private:

};
} // namespace Math

#endif // VECTOR3_H_INCLUDE