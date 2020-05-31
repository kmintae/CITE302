/**
 * Direction.h
 * Purpose: Class encapsulates 'Direction Information (2D/3D)'
 * @author Mintae Kim
 */

#pragma once

#include "Vector.h"

class Direction2D : public Vector2D
{
public:
	Direction2D() { };
	Direction2D(float dir_x, float dir_y) : Vector2D(dir_x, dir_y) { };
	Direction2D(Direction2D const& dir) : Vector2D(dir) { };

	void setDir2D(float x, float y);

	Direction2D getUnitVector();

	// Additional Operator
	const Direction2D operator* (const Direction2D& dir2);
};

// Inheriting Vector3D Only
class Direction3D : public Vector3D
{
public:
	Direction3D() { };
	Direction3D(float dir_x, float dir_y, float dir_z) : Vector3D(dir_x, dir_y, dir_z) { };
	Direction3D(Direction3D const& dir) : Vector3D(dir) { };

	Direction2D getDir2D();

	void setDir3D(float x, float y, float z);

	Direction3D getUnitVector();

	const Direction3D operator* (const Direction3D& dir2);
};

