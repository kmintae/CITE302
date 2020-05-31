/**
 * Brick.cpp
 * Purpose: Class encapsulates 'Brick'
 * @author Mintae Kim
 */

#include "Brick.h"

Brick::Brick()
{
	inSrc = true;
	phase = BrickPhase::ENABLE;
	unableTerritoryCnt = 0;
}

void Brick::setAsEnable()
{
	unableTerritoryCnt--;
	if (unableTerritoryCnt == 0)
		phase = BrickPhase::ENABLE;
}
void Brick::setAsUnable()
{
	unableTerritoryCnt++;
	phase = BrickPhase::UNABLE;
}
void Brick::setAsSelected()
{
	phase = BrickPhase::SELECTED;
}
void Brick::setAsGrabbed()
{
	phase = BrickPhase::GRABBED;
}
void Brick::setAsLifted()
{
	phase = BrickPhase::LIFTED;
}
void Brick::setAsReleased()
{
	phase = BrickPhase::RELEASED;
}
void Brick::setAsDone()
{
	phase = BrickPhase::DONE;
}

void Brick::setPos(float x, float y, float z)
{
	pose.first.setPos3D(x, y, z);
}
void Brick::setDir(float x, float y, float z)
{
	pose.second.setDir3D(x, y, z);
}

Position2D Brick::getPos2D()
{
	return pose.first.getPos2D();
}
Position3D Brick::getPos3D()
{
	return pose.first;
}
Direction2D Brick::getDir2D()
{
	return pose.second.getDir2D();
}
Direction3D Brick::getDir3D()
{
	return pose.second;
}


BrickPhase Brick::getPhase()
{
	return phase;
}

bool Brick::operator <(const Brick& b2)
{
	if (inSrc)
		return (pose.first < b2.pose.first);
	else
		return (pose.first > b2.pose.first);
}

std::string Brick::toString(bool printStatus)
{
	std::string brickInfo;
	if (printStatus) {
		brickInfo = "Brick ";
		brickInfo.append(pose.first.toString()).append(": ");
		if (inSrc) brickInfo.append("Non-Stacked");
		else brickInfo.append("Stacked");
	}
	else {
		brickInfo = pose.first.toString();
	}
	return brickInfo;
}