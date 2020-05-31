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