/**
 * Client.h
 * Purpose: Encapsulates Client Information (w/ Robot Object)
 * @author Mintae Kim
 */

#pragma once

#include <WinSock2.h>
#include <thread>

#include "../basic_blocks/Phase.h"
#include "../basic_blocks/Robot.h"

#include "../vector/Position.h"

class Client
{
private:
	Robot* robot;
	SOCKET sock;
	SOCKADDR_IN sock_in;

public:
	std::thread clientThread;
	ClientPhase phase;

	Client();
	~Client();

	void accept(SOCKET& sock, SOCKADDR_IN& sock_in);
	void connect(int robotNum, Position2D initialPos);
	
	SOCKET& getSocket();
	Robot* getRobot();
};