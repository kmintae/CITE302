
#pragma once

#include <utility>
#include <vector>

#include "../struct/vector/Vector.h"
#include "../struct/vector/Position.h"
#include "../struct/vector/Direction.h"

std::pair<Position2D, Direction2D> getKeypoint(std::pair<Position2D, Direction2D> pose, std::pair<Position2D, Direction2D> finalPose);

std::vector<std::pair<Position2D, Direction2D>> pathFinding(std::pair<Position2D, Direction2D> pose, std::pair<Position2D, Direction2D> finalPose);