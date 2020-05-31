/**
 * JsonParser.cpp
 * Purpose: Json Parser w/ nhloman JSON PARSER
 * @author Mintae Kim
 */

// Referencing: https://github.com/nlohmann/json
#include <nlohmann/json.hpp>

#include "JsonParser.h"

using json = nlohmann::json;

JsonParser::JsonParser()
{
	// TODO: Json Parsing
}

std::vector<BrickLayer*> JsonParser::getSrcBrickLayerList()
{
	return srcBrickLayerList;
}
std::vector<BrickLayer*> JsonParser::getDstBrickLayerList()
{
	return dstBrickLayerList;
}