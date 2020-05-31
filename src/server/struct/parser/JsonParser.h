/**
 * JsonParser.h
 * Purpose: Json Parser w/ nhloman JSON PARSER
 * @author Mintae Kim
 */

#pragma once

#include <vector>

#include "../basic_blocks/Brick.h"
#include "../basic_blocks/BrickLayer.h"

class JsonParser
{
private:
	std::vector<BrickLayer*> srcBrickLayerList, dstBrickLayerList;
public:
	JsonParser();

	std::vector<BrickLayer*> getSrcBrickLayerList();
	std::vector<BrickLayer*> getDstBrickLayerList();
};