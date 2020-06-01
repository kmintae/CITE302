/**
 * BrickLayerList.h
 * Purpose: Shared Class 'BrickLayerList': Manages Total Brick Layer & List
 * @author Mintae Kim
 */

#pragma once

#include <string>
#include <thread>
#include <mutex>
#include <vector>

#include "../basic_blocks/Phase.h"
#include "../basic_blocks/Brick.h"
#include "../basic_blocks/Robot.h"

#include "../parser/JsonParser.h"

#include "Grid.h"

#include "../vector/Position.h"
#include "../vector/Direction.h"

class BrickLayerList
{
private:
	std::vector<BrickLayer*> srcBrickLayerList, dstBrickLayerList;
	int srcBrickLayerIndex, dstBrickLayerIndex;

	std::mutex* mtx;
	std::condition_variable* cv;

public:
	BrickLayerList(std::mutex* mtx, std::condition_variable *cv);
	~BrickLayerList();

	Brick* getNextSrcBrick(Robot* robot, Grid* grid, static int& srcBrickLayerIndex);
	Brick* getNextDstBrick(Robot* robot, Grid* grid, static int& dstBrickLayerIndex);

	std::pair<Position2D, Direction2D> getFinalPose(Grid* grid, Brick* brick);

	void markAsSelectedBrick(int srcBrickLayerIndex, Brick* brick);
	void markAsGrabbedBrick(Brick* brick);
	void markAsLiftedBrick(int dstBrickLayerIndex, Brick* brick);
	void markAsReleasedBrick(Brick* brick);
	void markAsDone(int srcBrickLayerIndex, Brick* srcBrick, int dstBrickLayerIndex, Brick* dstBrick);

	bool isDone();
	float getProgressRate();
};