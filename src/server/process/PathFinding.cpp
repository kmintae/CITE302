/**
 * PathFinding.cpp
 * Purpose: Path Finding Algorithm
 * @author Mintae Kim
 * @author Youngtae Moon
 */

#include "PathFinding.h"

std::pair<Position2D, Direction2D> getKeypoint(std::pair<Position2D, Direction2D> pose, std::pair<Position2D, Direction2D> finalPose)
{
	std::vector<std::pair<Position2D, Direction2D>> keypointList;

	keypointList = pathFinding(pose, finalPose);

	if (keypointList.size() == 0) return NULL;
	return keypointList.front();
}

std::vector<std::pair<Position2D, Direction2D>> pathFinding(std::pair<Position2D, Direction2D> pose, std::pair<Position2D, Direction2D> finalPose)
{
	// TODO: Implement
	return NULL;
}