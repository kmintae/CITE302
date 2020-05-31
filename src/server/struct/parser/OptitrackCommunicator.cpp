/**
 * OptitrackCommunicator.cpp
 * Purpose: Communicator w/ Optitrack, Holding Position & Direction Data
 * @author Mintae Kim
 * @author Rafat Cieslak (https://rafalcieslak.wordpress.com/2014/05/16/c11-stdthreads-managed-by-a-designated-class/)
 */

#include "OptitrackCommunicator.h"

OptitrackCommunicator::OptitrackCommunicator() : communicator()
{
	int maxRobotLimit = GetPrivateProfileInt("connection", "MAX_ROBOT_CONNECTED", 2, "../../../config/server.ini");

	posArr = new Position2D[maxRobotLimit];
	dirArr = new Direction2D[maxRobotLimit];

	isDestructed = false;

	// Multithread function communicate()
	communicator = std::thread(&OptitrackCommunicator::communicate, this);
}
OptitrackCommunicator::~OptitrackCommunicator()
{
	// Lock Acquired
	std::unique_lock<std::mutex> lck(mtx);

	// TODO: Kill Thread
	isDestructed = true;

	communicator.join();

	delete[] posArr;
	delete[] dirArr;
}

void OptitrackCommunicator::communicate()
{
	while (true)
	{
		if (isDestructed) break;

		// TODO: Execution of OptitrackCommunicate Function

		// TODO: Parsing Data

		updateArray(); // TODO: param = data

		// Sleep for 0.5s
		Sleep(500);
	}
}
void OptitrackCommunicator::updateArray()
{
	// Lock Acquired
	std::unique_lock<std::mutex> lck(mtx);

	// TODO: Update Array
}

// Don't Forget to delete after using get Pos/Dir Array()
std::pair<Position2D, Direction2D>* OptitrackCommunicator::getPoseArray()
{
	// Lock Acquired
	std::unique_lock<std::mutex> lck(mtx);

	std::pair<Position2D, Direction2D>* newPoseArr = new std::pair<Position2D, Direction2D>[maxRobotLimit];
	for (int i = 0; i < maxRobotLimit; i++) newPoseArr[i] = poseArr[i];

	return newPoseArr;
}
std::pair<Position2D, Direction2D> OptitrackCommunicator::getPose(int robotNum)
{
	// Lock Acquired
	std::unique_lock<std::mutex> lck(mtx);

	return poseArr[robotNum];
}

static int OptitrackCommunicator::getConnectWaitingRobotNum(std::pair<Position2D, Direction2D>* prevPoseArr)
{
	// Lock Acquired
	std::unique_lock<std::mutex> lck(mtx);

	// TODO: Detect One with 22.5 Degree (Return -1 if Fails/Detects more than 2)
	// Information: prevPoseArr, poseArr

}