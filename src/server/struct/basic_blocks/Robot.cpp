/**
 * Robot.cpp
 * Purpose: Class encapsulates 'Robot' Information
 * @author Mintae Kim
 */

#include "Robot.h"

// TODO: Apply LOCK

Robot::Robot(int robotNum, pair<Position2D, Direction2D>& initPose)
{
	this->robotNum = robotNum;
	this->pose = initPose;

	phase = RobotPhase::STOP;
	src = dest = NULL;
}

int Robot::getRobotNum()
{
	return robotNum;
}

void Robot::setPose(std::pair<Position2D, Direction2D> pose)
{
	this->pose = pose;
}
std::pair<Position2D, Direction2D> Robot::getPose()
{
	return pose;
}
void Robot::setKeypoint(std::pair<Position2D, Direction2D> keypoint)
{
	this->keypoint = keypoint;
}
std::pair<Position2D, Direction2D> Robot::getKeypoint()
{
	return keypoint;
}
std::pair<Position2D, Direction2D> Robot::getFinalPose()
{
	return finalPose;
}

Brick* Robot::getSourceBrick()
{
	return src;
}
Brick* Robot::getDestinationBrick()
{
	return dest;
}

void Robot::markAsMove(Brick* srcBrick, std::pair<Position2D, Direction2D> finalPose)
{
	phase = RobotPhase::MOVING;
	src = srcBrick;
	this->finalPose = finalPose;
}
void Robot::markAsGrab()
{
	phase = RobotPhase::GRAB;
}
void Robot::markAsLift(Brick* dstBrick, std::pair<Position2D, Direction2D> finalPose)
{
	phase = RobotPhase::LIFTING;
	dst = dstBrick;
	this->finalPose = finalPose;
}
void Robot::markAsRelease()
{
	phase = RobotPhase::RELEASE;
}
void Robot::markAsStop()
{
	phase = RobotPhase::STOP;
	src = dst = NULL;
}

std::string Robot::toString()
{
	std::string str;

	switch (phase)
	{
	case RobotPhase::STOP:
		str = "Stopped";
		break;
	case RobotPhase::MOVING:
		str = "Moving";
		break;
	case RobotPhase::GRAB:
		"Grabbing";
		break;
	case RobotPhase::LIFTING:
		str = "Lifting";
		break;
	case RobotPhase::RELEASE:
		str = "Releasing";
		break;
	}

	str.append(" ").append("Pos: ").append(pos_cur.toString()).append(", Dir: ").append(dir_cur.toString());

}