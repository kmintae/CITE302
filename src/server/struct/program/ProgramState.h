/**
 * ProgramState.h
 * Purpose: Class w/ Overall Program Information w/ Robot List
 * @author Mintae Kim
 */

#pragma once

#include <queue>
#include <Windows.h>
#include <thread>
#include <mutex>
#include <string>
#include <utility>
#include <stdio.h>

#include "../basic_blocks/Instruction.h"
#include "../basic_blocks/Phase.h"
#include "../basic_blocks/Robot.h"

#include "../connection/Client.h"

#include "../parser/OptitrackCommunicator.h"

#include "../states/BrickLayerList.h"
#include "../states/Grid.h"

#include "../vector/Direction.h"
#include "../vector/Position.h"
#include "../vector/Direction.h"

#define MAX_BUFF_SIZE 512

class ProgramState
{
private:
	// Robot Lists
	Client* clients;

	// Waiting Buffer
	static std::queue<Client*> waitingBuffer;

	// OptitrackCommunicator
	static OptitrackCommunicator optitrackCommunicator;

	// Status: BrickListManager, Grid
	BrickLayerList* brickLayerList;
	int srcBrickLayerIndex, dstBrickLayerIndex;
	Grid* grid;

	// Mutex & Condition Variable
	std::mutex mtx_state;
	std::condition_variable cv_state;
	
	std::mutex mtx_connect;

	// Variables
	bool isWorking; // Current Program State
	int connClientNum, maxClientNum;

public:
	ProgramState();
	~ProgramState();

	// Related to Connection
	void acceptClient(SOCKET* serverSock); // Executed in Independent Thread
	void optitrackConnect(); // Executed in Independent Thread

	void workSession(Client *client); // Executed in independent thread (One Thread per Connected Client)
};