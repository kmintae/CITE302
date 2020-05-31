/**
 * ProgramState.cpp
 * Purpose: Class w/ Overall Program Information w/ Robot List
 * @author Mintae Kim
 * @author chrisysl (https://kevinthegrey.tistory.com/26)
 */

#include "ProgramState.h"

ProgramState::ProgramState()
{
	brickListManager = new BrickListManager(&mtx_state, &cv_state);
	grid = new Grid(&mtx_state, &cv_state);
	isWorking = false;
	connClientNum = 0;

	maxClientNum = GetPrivateProfileInt("connection", "MAX_ROBOT_CONNECTED", 2, "../../../config/server.ini");

	clients = new Client[maxClientNum];

	// Multi-Threading
}
ProgramState::~ProgramState()
{

	// De-Allocation
	delete brickListManager;
	delete grid;
	delete[] clients;
}

// Executed in Independent Thread
void ProgramState::acceptClient(SOCKET* serverSock)
{
	SOCKADDR_IN tClntAddr = {};
	int iClntSize = sizeof(tClntAddr);
	SOCKET hClient = accept(hListen, (SOCKADDR*)&tClntAddr, &iClntSize);

	// Lock Acquired
	std::unique_lock<std::mutex> lck(mtx_connect);

	// Checking Current Client Number
	if (connClientNum == maxClientNum) {
		closesocket(hClient);
		return;
	}

	// Making new Client Object
	Client* newClient = new Client();
	newClient->accept(hClient, tClntAddr);

	// Push newClient into connQueue
	ProgramState::waitingBuffer.push(newClient);
}

void ProgramState::optitrackConnect()
{
	// Lock Acquired
	std::unique_lock<std::mutex> lck(mtx_connect); // Updating connQueue
	
	// Assumption: Connection will be held without conflicts incurred by other robots
	while (!waitingBuffer.empty())
	{
		Client* head = waitingBuffer.front();
		// Checking Current Client Number
		if (connClientNum == maxClientNum) {
			delete head;
		}
		else {
			bool socketTerminated = false;
			int robotNum;
			Position2D initialPos;
			while (true)
			{
				// Try Connection
				std::pair<Position2D, Direction2D>* prevPoseArr = ProgramState::optitrackCommunicator.getPoseArray();

				// Sending 'CAL' Instruction
				SOCKET socket = head->getSocket();
				Instruction(InstructionType::CAL, NULL).toString();
				int bytes_send = send(socket, Instruction(InstructionType::CAL, NULL).toString().c_str(), MAX_BUFF_SIZE, 0);
				if (bytes_send == SOCKET_ERROR) {
					fprintf(stderr, "Send Failed, Termination of Socket Connection\n");
					delete[] prevPoseArr;

					socketTerminated = true;
					closesocket(socket);
					break;
				}

				// Receiving Done
				char buff[MAX_BUFF_SIZE] = { 0, };
				int bytes_recv = recv(socket, buff, MAX_BUFF_SIZE, 0);
				if (bytes_send == SOCKET_ERROR) {
					fprintf(stderr, "Send Failed, Termination of Socket Connection\n");
					delete[] prevPoseArr;

					socketTerminated = true;
					closesocket(socket);
					break;
				}

				// Determination of Corresponding Robot Number from OptitrackCommunicator
				robotNum = ProgramState::optitrackCommunicator.getConnectWaitingRobotNum(prevPoseArr);
				
				delete[] prevPoseArr;
				
				if (robotNum == -1) continue;
				
				// Check if robot_num is already allocated
				bool isUsing = false;
				std::unique_lock<std::mutex> lck2(mtx_state);
				if (clients[robotNum].phase != ClientPhase::DISCONNECTED) isUsing = true;
				lck2.unlock();
				
				if (!isUsing) {
					initialPos = ProgramState::optitrackCommunicator.getPose(robotNum).first;
					break;
				}
			}
			
			// Update Array
			if (!socketTerminated)
			{
				std::unique_lock<std::mutex> lck2(mtx_state);
				head->connect(robotNum, initialPos);
				clients[robotNum] = head;
				// Start Multi-threading
				clients[robotNum].clientThread = std::thread(&ProgramState::workSession, this, clients[robotNum]);
				lck2.unlock();
			}
		}
		waitingBuffer.pop();
	}
}

void ProgramState::workSession(Client* client)
{
	// TODO: Implement Workflow
}