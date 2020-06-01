/**
 * Robot.h
 * Purpose: Class encapsulates 'Robot' Information
 * @author Mintae Kim
 */

#pragma once

#include <string>
#include <utility>
#include <thread>
#include <mutex>

#include "Phase.h"
#include "Brick.h"
#include "BrickLayer.h"
#include "Instruction.h"

#include "../vector/Position.h"
#include "../vector/Direction.h"

class Robot
{
private:
	int robotNum;
	
	std::pair<Position2D, Direction2D> pose, keypoint, finalPose;
	Brick* src, dest;

	// Lock: Need Information?
	std::mutex mtx;

public:
	RobotPhase phase;

	Robot(int robotNum, pair<Position2D, Direction2D>& initPose);

	int getRobotNum();

	void setPose(std::pair<Position2D, Direction2D> pose);
	std::pair<Position2D, Direction2D> getPose();
	void setKeypoint(std::pair<Position2D, Direction2D> keypoint);
	std::pair<Position2D, Direction2D> getKeypoint();
	
	std::pair<Position2D, Direction2D> getFinalPose();

	Brick* getSourceBrick();
	Brick* getDestinationBrick();

	void markAsMove(Brick* srcBrick, std::pair<Position2D, Direction2D> finalPose);
	void markAsGrab();
	void markAsLift(Brick* dstBrick, std::pair<Position2D, Direction2D> finalPose);
	void markAsRelease();
	void markAsStop();

	std::string toString();
};