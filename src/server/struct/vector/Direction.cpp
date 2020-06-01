/**
 * Direction.cpp
 * Purpose: Class encapsulates 'Direction Information'
 * @author Mintae Kim
 */

#include "Direction.h"

// 2D
void Direction2D::setDir2D(float x, float y)
{
	Vector2D::setVect2D(x, y);
}

Direction2D Direction2D::getUnitVector()
{
	return *this / (*this->size());
}

// Operator Overloading
const Direction2D Direction2D::operator +(const Direction2D& dir2)
{
	float rad1 = Direction2D::vectorToRadian(*this);
	float rad2 = Direction2D::vectorToRadian(dir2);

	return Direction2D::radianToVector(rad1 + rad2);
}
Direction2D& Direction2D::operator +=(const Direction2D& dir2)
{
	float rad1 = Direction2D::vectorToRadian(*this);
	float rad2 = Direction2D::vectorToRadian(dir2);

	Direction2D sum = Direction2D::radianToVector(rad1 + rad2);
	x = sum.x;
	y = sum.y;
	
	return *this;
}
const Direction2D Direction2D::operator -(const Direction2D& dir2)
{
	float rad1 = Direction2D::vectorToRadian(*this);
	float rad2 = Direction2D::vectorToRadian(dir2);

	return Direction2D::radianToVector(rad1 - rad2);
}
Direction2D& Direction2D::operator -=(const Direction2D& dir2)
{
	float rad1 = Direction2D::vectorToRadian(*this);
	float rad2 = Direction2D::vectorToRadian(dir2);

	Direction2D sub = Direction2D::radianToVector(rad1 - rad2);
	x = sub.x;
	y = sub.y;

	return *this;
}
const Direction2D Direction2D::operator +(const float rad2)
{
	float rad1 = Direction2D::vectorToRadian(*this);
	return Direction2D::radianToVector(rad1 + rad2);
}
Direction2D& Direction2D::operator +=(const float rad2)
{
	float rad1 = Direction2D::vectorToRadian(*this);

	Direction2D sum = Direction2D::radianToVector(rad1 + rad2);
	x = sum.x;
	y = sum.y;

	return *this;
}
const Direction2D Direction2D::operator -(const float rad2)
{
	float rad1 = Direction2D::vectorToRadian(*this);

	return Direction2D::radianToVector(rad1 - rad2);
}
Direction2D& Direction2D::operator -=(const float rad2)
{
	float rad1 = Direction2D::vectorToRadian(*this);

	Direction2D sub = Direction2D::radianToVector(rad1 - rad2);
	x = sub.x;
	y = sub.y;

	return *this;
}
bool Direction2D::operator ==(const Direction2D& dir2)
{
	return ((abs(Direction2D::vectorToRadian(*this - dir2))) < errorLimit);
}
static Direction2D Direction2D::radianToVector(float radian)
{
	return Direction2D(cos(radian), sin(radian));
}
static float Direction2D::vectorToRadian(const Direction2D& unitVector)
{
	return atan2(unitVector.y, unitVector.x);
}

// Vector
const Direction2D Direction2D::operator* (const Direction2D& dir2)
{
	return dir_x * dir2.dir_x + dir_y * dir2.dir_y;
}


// 3D
void Direction3D::setDir3D(float x, float y, float z)
{
	Vector3D::setVect3D(x, y, z);
}

Direction2D Direction3D::getDir2D()
{
	return Vector3D::getVect2D();
}

Direction3D Direction3D::getUnitVector()
{
	return *this / (*this->size());
}

// Operator Overloading
// Vector
const Direction3D Direction3D::operator* (const Direction3D& dir2)
{
	return dir_x * dir2.dir_x + dir_y * dir2.dir_y + dir_z * dir2.dir_z;
}