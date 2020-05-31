/**
 * Grid.cpp
 * Purpose: Shared Class 'Grid': Expressing Current State
 * @author Mintae Kim
 */

#include "Grid.h"

Grid::Grid(std::mutex* mtx, std::condition_variable* cv)
{
	this->mtx = mtx;
	this->cv = cv;

	int limit_x, limit_y, grid_len;
	limit_x = GetPrivateProfileInt("limit", "X_MM", -1, "../../../config/server.ini");
	limit_y = GetPrivateProfileInt("limit", "Y_MM", -1, "../../../config/server.ini");
	grid_len = GetPrivateProfileInt("grid", "GRID_LEN_MM", 0, "../../../config/server.ini");

	x = ((limit_x % grid_len == 0) ? (limit_x / grid_len) : (limit_x / grid_len + 1));
	y = ((limit_y % grid_len == 0) ? (limit_y / grid_len) : (limit_y / grid_len + 1));

	// Dynamic Allocation
	grid = new bool* [y];
	
	for (int i = 0; i < y; i++) grid[i] = new bool[x];
}

Grid::~Grid()
{
	for (int i = 0; i < y; i++) delete [] grid[i];
	delete[] grid;
}

bool** Grid::getGrid()
{
	// Lock Acquired
	std::unique_lock<std::mutex> lck(*mtx);

	return grid;
}

void Grid::repaint(BrickLayerList* brickLayerList, OptitrackCommunicator* optitrackCommunicator, Client* client_list)
{
	// TODO: DRAW GRID! (BrickList, Optitrack Infos, Client List (Path))

}