/**
 * Position.h
 * Purpose: Class encapsulates 'Position Information (2D/3D)'
 * @author Mintae Kim
 */

#pragma once

#include <string>
#include <cmath>

#include "Vector.h"

class Position2D : public Vector2D
{
public:
	Position2D() { };
	Position2D(float pos_x, float pos_y) : Vector2D(pos_x, pos_y) { };
	Position2D(Position2D const& pos) : Vector2D(pos) { };

	static float calculateDistance(const Position2D& vect1, const Position2D& vect2);

	void setPos2D(float x, float y);
};

// Only Inherits Vector3D
class Position3D : public Vector3D
{
public:
	Position3D() { };
	Position3D(float pos_x, float pos_y, float pos_z) : Vector3D(pos_x, pos_y, pos_z){ };
	Position3D(Position3D const& pos) : Vector3D(pos){ };

	Position2D getPos2D();

	static float calculateDistance(const Position3D& vect1, const Position3D& vect2);

	void setPos3D(float x, float y, float z);

	// Additional Operator Overloading
	bool operator <(const Position3D& pos2);
	bool operator >(const Position3D& pos2);
};