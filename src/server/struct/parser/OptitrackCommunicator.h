/**
 * OptitrackCommunicator.h
 * Purpose: Communicator w/ Optitrack, Holding Position & Direction Data
 * @author Mintae Kim
 */

#pragma once

#include <thread>
#include <mutex>
#include <Windows.h>
#include <utility>
#include <string>
#include <cmath>

#include "../../process/OptitrackCommunicate.h"

#include "../vector/Position.h"
#include "../vector/Direction.h"

class OptitrackCommunicator
{
private:
	int maxRobotLimit;

	std::thread communicator;

	std::pair<Position2D, Direction2D>* poseArr; // Shared: Heap

	static std::mutex mtx; // Shared
	static bool isDestructed; // Shared

	void updateArray(std::string rawData);

	static SOCKET udpSocket;

public:
	OptitrackCommunicator();
	~OptitrackCommunicator();

	void communicate(); // Parsing Data

	std::pair<Position2D, Direction2D>* getPoseArray();

	std::pair<Position2D, Direction2D> getPose(int robotNum);

	static int getConnectWaitingRobotNum(std::pair<Position2D, Direction2D>* prevPoseArr);
};