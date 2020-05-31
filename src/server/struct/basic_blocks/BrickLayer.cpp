/**
 * BrickLayer.cpp
 * Purpose: Class encapsulates 'BrickLayer', Collection of Bricks in 'Same Level'
 * @author Mintae Kim
 */

#include "BrickLayer.h"

BrickLayer::BrickLayer()
{
	radius = 0.0;
}
BrickLayer::~BrickLayer()
{
	// Deallocate All Bricks
	for (int i = 0; i < brickList.size(); i++) {
		delete brickList[i];
	}
}

void BrickLayer::calculateCenter()
{
	Position2D center(0, 0);
	for (int i = 0; i < brickList.size(); i++)
		center = center + brickList[i]->getPos2D();
	center = center / brickList.size();
	center.callibrate(); // < 0.0001 Ȯ��
	layerCenter = center;
}
void BrickLayer::calculateRadius()
{
	radius = Position2D::calculateDistance(brickList[0]->getPos2D(), layerCenter);
}

void BrickLayer::addBrick(Brick *nb)
{
	brickList.push_back(nb);
	readyList.push_back(nb);
}
Position2D BrickLayer::getCenter()
{
	return layerCenter;
}

std::vector<Brick*> BrickLayer::getBrickList(){
	return brickList;
}
std::vector<Brick*> BrickLayer::getEnableBrickList()
{
	std::vector<Brick*> enableBrickList;
	for (int i = 0; i < brickList.size(); i++) {
		if (brickList[i]->getPhase() == BrickPhase::ENABLE) enableBrickList.push_back(brickList[i]);
	}
	return enableBrickList;
}
std::vector<Brick*> BrickLayer::getStackedList()
{
	return stackedList;
}

void BrickLayer::markAsSelected(Brick* b)
{
	int bodysize = GetPrivateProfileInt("physical", "ROBOT_BODY_SIZE_MM", 0, "../../../config/server.ini");

	b->setAsSelected();
	ongoingList.push_back(b);

	// Remove b in readyList
	std::vector<Brick*> new_readyList;
	for (int i = 0; i < readyList.size(); i++)
		if (readyList[i] != b) new_readyList.push_back(readyList[i]);

	for (int i = 0; i < brickList.size(); i++)
		if (b->getPos3D() != brickList[i]->getPos3D())
			if (b->getPhase() == BrickPhase::ENABLE)
				if (Position2D::calculateDistance(b->getPos2D(), brickList[i]->getPos2D()) < bodysize)
					b->setAsUnable();
}
void BrickLayer::markAsDone(Brick* b)
{
	int bodysize = GetPrivateProfileInt("physical", "ROBOT_BODY_SIZE_MM", 0, "../../../config/server.ini");

	b->setAsDone();
	stackedList.push_back(b);

	// Remove b in ongoingList
	std::vector<Brick*> new_ongoingList;
	for (int i = 0; i < ongoingList.size(); i++)
		if (ongoingList[i] != b) new_ongoingList.push_back(ongoingList[i]);

	for (int i = 0; i < brickList.size(); i++)
		if (b->getPos3D() != brickList[i]->getPos3D())
			if (b->getPhase() == BrickPhase::ENABLE)
				if (Position2D::calculateDistance(b->getPos2D(), brickList[i]->getPos2D()) < bodysize)
					b->setAsEnable();
}

bool BrickLayer::isDone()
{
	return (brickList.size() == stackedList.size());
}
bool BrickLayer::getRadius() // Radius of Circle
{
	return radius;
}

std::string BrickLayer::toString()
{
	std::string str = "Layer: \n";
	str.append("[N-Stacked] ");
	for (int i = 0; i < readyList.size(); i++) str.append(readyList[i]->toString(false)).append(" ");
	str.append("\n[On-going] ");
	for (int i = 0; i < ongoingList.size(); i++) str.append(ongoingList[i]->toString(false)).append(" ");
	str.append("\n[Stacked] ");
	for (int i = 0; i < stackedList.size(); i++) str.append(stackedList[i]->toString(false)).append(" ");
	str.append("\n ");
	return str;
}