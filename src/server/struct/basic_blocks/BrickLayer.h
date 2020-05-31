/**
 * BrickLayer.h
 * Purpose: Class encapsulates 'BrickLayer', Collection of Bricks in 'Same Level'
 * @author Mintae Kim
 */

#pragma once

#include <string>
#include <vector>
#include <Windows.h>

#include "Phase.h"
#include "Brick.h"

#include "../vector/Position.h"

class BrickLayer
{
private:
    Position2D layerCenter;
    std::vector<Brick*> brickList;
    std::vector<Brick*> readyList, ongoingList, stackedList;

    float radius; // Radius of Circle

    void calculateCenter();
    void calculateRadius();
public:
    BrickLayer();
    ~BrickLayer();

    Position2D getCenter();

    void addBrick(Brick *nb);
    std::vector<Brick*> getBrickList();
    std::vector<Brick*> getEnableBrickList();
    std::vector<Brick*> getStackedList();

    void markAsSelected(Brick* b);
    void markAsDone(Brick* b);

    bool isDone();
    
    bool getRadius(); // Radius of Circle

    std::string toString();
};