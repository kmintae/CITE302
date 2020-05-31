/**
 * Grid.h
 * Purpose: Shared Class 'Grid': Expressing Current State
 * @author Mintae Kim
 */

#pragma once

#include <mutex>
#include <thread>
#include <Windows.h>

#include "../basic_blocks/Robot.h"

#include "../connection/Client.h"

#include "../parser/OptitrackCommunicator.h"

#include "BrickLayerList.h"

#include "../vector/Vector.h"
#include "../vector/Position.h"
#include "../vector/Direction.h"

class Grid
{
private:
	bool** grid;
	
	std::mutex* mtx;
	std::condition_variable* cv;

public:
	int x, y;
	Grid(std::mutex* mtx, std::condition_variable* cv);
	~Grid();

	bool** getGrid();

	void repaint(BrickLayerList *brickLayerList, OptitrackCommunicator* optitrackCommunicator, Client* client_list);
};