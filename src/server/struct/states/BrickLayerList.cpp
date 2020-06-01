/**
 * BrickLayerList.cpp
 * Purpose: Shared Class 'BrickLayerList': Manages Total Brick Layer & List
 * @author Mintae Kim
 */

#include "BrickLayerList.h"

BrickLayerList::BrickLayerList(std::mutex *mtx, std::condition_variable* cv)
{
	JsonParser jsonParser = JsonParser(); // Loading Json

	srcBrickLayerList = jsonParser.getSrcBrickLayerList();
	dstBrickLayerList = jsonParser.getDstBrickLayerList();

	this->mtx = mtx;
	this->cv = cv;
}
BrickLayerList::~BrickLayerList()
{
	// De-Allocate Brick Layers
	for (int i = 0; i < srcBrickLayerList.size(); i++)
		delete srcBrickLayerList[i];
	for (int i = 0; i < dstBrickLayerList.size(); i++)
		delete dstBrickLayerList[i];
}

void BrickLayerList::getNextSrcBrick(Robot* robot, Grid* grid, static int &srcBrickLayerIndex)
{
	// Searching by O(N)
	// Considering LayerList & Grid
	Brick* result = NULL;
	Position2D robotPosition = robot->getPosition();
	int** grid = Grid.getGrid();

	while (true)
	{
		// Is Current Layer Jobs Done?
		if (srcBrickLayerList[this->srcBrickLayerIndex]->isDone()) {
			if (srcBrickLayerIndex == srcBrickLayerList.size() - 1) return; // Jobs Done
			else srcBrickLayerIndex++;
		}
		
		// TODO: Search for Bricks
		
		if (result == NULL) (*cv).wait(lck);
		else break;
	}
	srcBrickLayerIndex = this->srcBrickLayerIndex;
	return result;
}
Brick* BrickLayerList::getNextDstBrick(Robot* robot, Grid* grid, static int &dstBrickLayerIndex)
{
	// Searching by O(N)
	// Considering LayerList & Grid
	Brick* result = NULL;
	Position2D robotPosition = robot->getPosition();
	int** grid = Grid.getGrid();

	while (true)
	{
		// Is Current Layer Jobs Done?
		if (dstBrickLayerList[dstBrickLayerIndex]->isDone()) {
			if (dstBrickLayerIndex == dstBrickLayerList.size() - 1) return NULL; // Jobs Done
			else dstBrickLayerIndex++;
		}

		// TODO: Implement
		
		if (result == NULL) (*cv).wait(lck);
		else break;
	}
	dstBrickLayerIndex = this->dstBrickLayerIndex;
	return result;
}

std::pair<Position2D, Direction2D> BrickLayerList::getFinalPose(Grid* grid, Brick* brick)
{
	// TODO: Get Final Pose w/ Grid & Brick Information

}

void BrickLayerList::markAsSelectedBrick(int srcBrickLayerIndex, Brick* brick)
{
	srcBrickLayerList[srcBrickLayerIndex]->markAsSelected(brick);
}
void BrickLayerList::markAsGrabbedBrick(Brick* brick)
{
	brick->setAsGrabbed();
}
void BrickLayerList::markAsLiftedBrick(int dstBrickLayerIndex, Brick* brick)
{
	dstBrickLayerList[dstBrickLayerIndex]->markAsSelected(brick);
}
void BrickLayerList::markAsReleasedBrick(Brick* brick)
{
	brick->setAsReleased();
}
void BrickLayerList::markAsDone(int srcBrickLayerIndex, Brick *srcBrick, int dstBrickLayerIndex, Brick *dstBrick)
{
	srcBrickLayerList[srcBrickLayerIndex]->markAsDone(srcBrick);
	dstBrickLayerList[dstBrickLayerIndex]->markAsDone(dstBrick);
}

bool BrickLayerList::isDone()
{
	// Lock Acquired
	std::unique_lock<std::mutex> lck(*mtx);

	int bricksTotal = 0;
	for (int i = 0; i < dstBrickLayerList.size(); i++) {
		bricksTotal += dstBrickLayerList[i]->getBrickList().size();
	}

	int bricksDone = 0;
	for (int i = 0; i < dstBrickLayerIndex; i++) {
		bricksDone += dstBrickLayerList[i]->getStackedList().size();
	}
	for (int i = 0; i < dstBrickLayerList[dstBrickLayerIndex]->getBrickList().size(); i++) {
		if (dstBrickLayerList[dstBrickLayerIndex]->getBrickList()[i]->getPhase() == BrickPhase::DONE)
			bricksDone++;
	}

	if (bricksDone == bricksTotal) return true;
	else return false;
}

float BrickLayerList::getProgressRate()
{
	// Lock Acquired
	std::unique_lock<std::mutex> lck(*mtx);

	int bricksTotal = 0;
	for (int i = 0; i < dstBrickLayerList.size(); i++) {
		bricksTotal += dstBrickLayerList[i]->getBrickList().size();
	}

	int bricksDone = 0;
	for (int i = 0; i < dstBrickLayerIndex; i++) {
		bricksDone += dstBrickLayerList[i]->getStackedList().size();
	}
	for (int i = 0; i < dstBrickLayerList[dstBrickLayerIndex]->getBrickList().size(); i++) {
		if (dstBrickLayerList[dstBrickLayerIndex]->getBrickList()[i]->getPhase() == BrickPhase::DONE)
			bricksDone++;
	}

	return (float)bricksDone * 100.0 / (float)bricksTotal;
}