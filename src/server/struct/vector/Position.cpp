/**
 * Position.cpp
 * Purpose: Class encapsulates 'Position Information'
 * @author Mintae Kim
 */

#include "Position.h"

// 2D
void Position2D::setPos2D(float x, float y)
{
	Vector2D::setVect2D(x, y);
}
static float Position2D::calculateDistance(const Position2D& vect1, const Position2D& vect2)
{
	float res = pow((vect1.x - vect2.x), 2.0) + pow((vect1.y - vect2.y), 2.0);
	res = pow(res, 0.5);
	return res;
}


// 3D
Position2D Position3D::getPos2D()
{
	return Vector3D::getVect2D();
}

void Position3D::setPos3D(float x, float y, float z)
{
	Vector3D::setVect3D(x, y, z);
}
static float Position3D::calculateDistance(const Position3D& vect1, const Position3D& vect2)
{
	float res = pow((vect1.x - vect2.x), 2.0) + pow((vect1.y - vect2.y), 2.0);
	res = pow(res, 0.5);
	return res;
}

bool Position3D::operator <(const Position3D& pos2)
{
	if (z < pos2.z) return true;
	if (z > pos2.z) return false;

	if (y < pos2.y) return true;
	if (y > pos2.y) return false;

	if (x < pos2.x) return true;
	if (x > pos2.x) return false;
}
bool Position3D::operator >(const Position3D& pos2)
{
	return (!(*this < pos2) && !(*this == pos2));
}