/**
 * ProgramState.cpp
 * Purpose: Class w/ Overall Program Information w/ Robot List
 * @author Mintae Kim
 * @author chrisysl (https://kevinthegrey.tistory.com/26)
 */

#include "ProgramState.h"

ProgramState::ProgramState() : thread_conn()
{
	brickListManager = new BrickListManager(&mtx_state, &cv_state);
	grid = new Grid(&mtx_state, &cv_state);

	ProgramState::isTerminationActivated = false;
	ProgramState::isWorking = false;
	ProgramState::connClientNum = 0;

	ProgramState::maxClientNum = GetPrivateProfileInt("connection", "MAX_ROBOT_CONNECTED", 2, "../../../config/server.ini");

	clients = new Client[ProgramState::maxClientNum];

	ProgramState::srcBrickLayerIndex = ProgramState::dstBrickLayerIndex = 0;

	// Multi-Threading
	thread_conn = std::thread(&ProgramState::optitrackConnect, this);
}
ProgramState::~ProgramState()
{
	// Thread Joining
	thread_conn.join();
	for (int i = 0; i < ProgramState::maxClientNum; i++) {
		if (clients[i].phase == ClientPhase::ACCEPTED) clients[i].clientThread.join();
	}
	
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

	// Emergency Exit
	// Lock Acquired
	std::unique_lock<std::mutex> lck_program(mtx_program);
	if (ProgramState::isTerminationActivated) {
		closesocket(hClient);
		return;
	}
	lck_program.unlock();

	// Lock Acquired
	std::unique_lock<std::mutex> lck(mtx_connect);

	// Checking Current Client Number
	if (ProgramState::connClientNum == ProgramState::maxClientNum) {
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
	while (true)
	{
		// Emergency Exit
		// Lock Acquired
		std::unique_lock<std::mutex> lck_program(mtx_program);
		if (ProgramState::isTerminationActivated) {
			return;
		}
		lck_program.unlock();

		// Lock Acquired
		std::unique_lock<std::mutex> lck(mtx_connect); // Updating connQueue

		if (brickLayerList->isDone()) {
			break;
		}

		// Assumption: Connection will be held without conflicts incurred by other robots
		while (!waitingBuffer.empty())
		{
			Client* head = waitingBuffer.front();
			// Checking Current Client Number
			if (ProgramState::connClientNum == ProgramState::maxClientNum) {
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
}

void ProgramState::workSession(Client* client)
{
	while (true)
	{
		// Emergency Exit
		// Lock Acquired
		std::unique_lock<std::mutex> lck_program(mtx_program);
		if (ProgramState::isTerminationActivated) {
			return;
		}
		lck_program.unlock();

		// Implement Workflow
		// 1. Update Position Information
		Robot* robot = client->getRobot();
		std::pair<Position2D, Direction2D> pose = ProgramState::optitrackCommunicator.getPose(robot->getRobotNum());
		robot->setPose(pose);

		// 2. Classify 
		switch (robot->phase)
		{
		case RobotPhase::STOP: // 2-1. STOP: Decision of srcBrick
			std::unique_lock<std::mutex> lck(mtx_state);
			Brick* srcBrick = brickLayerList->getNextSrcBrick(robot, grid, ProgramState::srcBrickLayerIndex);
			
			// All Jobs Done: Disconnect
			if (srcBrick == NULL) {
				SOCKET socket = client->getSocket();
				int bytes_send = send(socket, Instruction(InstructionType::DCN, NULL).toString().c_str(), MAX_BUFF_SIZE, 0);
				if (bytes_send == SOCKET_ERROR) {
					fprintf(stderr, "Send Failed, Termination of Socket Connection\n");
				}
				delete client; // closeSocket + De-allocation

				client = new Client();
				grid->repaint(brickLayerList, optitrackCommunicator, clients);
				return;
			}

			std::pair<Position2D, Direction2D> srcFinalPose = brickLayerList->getFinalPose(grid, srcBrick);
			robot->markAsMove(srcBrick, srcFinalPose);
			grid->repaint(brickLayerList, optitrackCommunicator, clients);
			break;


		case RobotPhase::MOVING: // 2-2. MOVING: Decision of Path, Saving Keypoint, and sending Instructions (MOV, PID)
			std::unique_lock<std::mutex> lck(mtx_state);
			
			// 2-2-1. Check if Current Pose == Final Pose: Send HLT, change phase to GRAB, and break
			if (robot->getPose() == robot->getFinalPose()) { // Current Pose == Final Pose
				// Send HLT
				SOCKET socket = client->getSocket();
				int bytes_send = send(socket, Instruction(InstructionType::HLT, NULL).toString().c_str(), MAX_BUFF_SIZE, 0);
				if (bytes_send == SOCKET_ERROR) {
					fprintf(stderr, "Send Failed, Termination of Socket Connection\n");
					
					delete client; // closeSocket + De-allocation
					client = new Client();
					grid->repaint(brickLayerList, optitrackCommunicator, clients);
					return;
				}

				// Change Phase to Grab
				robot->markAsGrab();
				grid->repaint(brickLayerList, optitrackCommunicator, clients);
				break;
			}

			// 2-2-2. Check if Current Pose == Keypoint: Send HLT, Pathfinding, Send PID, Receive DONE/ERROR, and break
			if (robot->getPose() == robot->getKeypoint()) { // Current Pose == Keypoint
				// Send HLT
				SOCKET socket = client->getSocket();
				int bytes_send = send(socket, Instruction(InstructionType::HLT, NULL).toString().c_str(), MAX_BUFF_SIZE, 0);
				if (bytes_send == SOCKET_ERROR) {
					fprintf(stderr, "Send Failed, Termination of Socket Connection\n");

					delete client; // closeSocket + De-allocation
					client = new Client();
					grid->repaint(brickLayerList, optitrackCommunicator, clients);
					return;
				}

				// Pathfinding, Save Keypoint in Robot
				std::pair<Position2D, Direction2D> pose = robot->getPose();
				std::pair<Position2D, Direction2D> keypoint = getKeyPoint(pose, robot->getFinalPose());
				robot->setKeypoint(keypoint);
				
				grid->repaint(brickLayerList, optitrackCommunicator, clients);
				
				lck.unlock();

				// Send PID
				float param[PID_PARAM] = { pose.first.x, pose.first.y, pose.second.x, pose.second.y, keypoint.first.x, keypoint.first.y, keypoint.second.x, keypoint.second.y };
				int bytes_send = send(socket, Instruction(InstructionType::PID, param).toString().c_str(), MAX_BUFF_SIZE, 0);
				if (bytes_send == SOCKET_ERROR) {
					fprintf(stderr, "Send Failed, Termination of Socket Connection\n");

					lck.lock();
					delete client; // closeSocket + De-allocation
					client = new Client();
					grid->repaint(brickLayerList, optitrackCommunicator, clients);
					return;
				}

				// Receive DONE/ERROR
				char buff[MAX_BUFF_SIZE] = { 0, };
				int bytes_recv = recv(socket, buff, MAX_BUFF_SIZE, 0);
				if (bytes_recv == SOCKET_ERROR) {
					fprintf(stderr, "Send Failed, Termination of Socket Connection\n");

					lck.lock();
					delete client; // closeSocket + De-allocation
					client = new Client();
					grid->repaint(brickLayerList, optitrackCommunicator, clients);
					return;
				}
				if (buff == "ERROR") {
					fprintf(stderr, "PID Initialization Failed\n");
				}

				break;
			}
			
			// 2-2-3. Send 'MOV' Instruction
			std::pair<Position2D, Direction2D> pose = robot->getPose();
			float param[PID_PARAM] = { pose.first.x, pose.first.y, pose.second.x, pose.second.y, 0, };
			int bytes_send = send(socket, Instruction(InstructionType::MOV, param).toString().c_str(), MAX_BUFF_SIZE, 0);
			if (bytes_send == SOCKET_ERROR) {
				fprintf(stderr, "Send Failed, Termination of Socket Connection\n");

				lck.lock();
				delete client; // closeSocket + De-allocation
				client = new Client();
				grid->repaint(brickLayerList, optitrackCommunicator, clients);
				return;
			}

			break;
		
		
		case RobotPhase::GRAB: // 2-3. GRAB: Sending 'GRB' Instruction, and decision of dstBrick
			// 2-3-1. Send GRB, and receive 'DONE'
			// Send GRB
			
			float param[PID_PARAM] = {}; // TODO: Calculate (R, Z)
			int bytes_send = send(socket, Instruction(InstructionType::GRB, param).toString().c_str(), MAX_BUFF_SIZE, 0);
			if (bytes_send == SOCKET_ERROR) {
				fprintf(stderr, "Send Failed, Termination of Socket Connection\n");

				std::unique_lock<std::mutex> lck(mtx_state);
				delete client; // closeSocket + De-allocation
				client = new Client();
				grid->repaint(brickLayerList, optitrackCommunicator, clients);
				return;
			}

			// Receive DONE/ERROR
			char buff[MAX_BUFF_SIZE] = { 0, };
			int bytes_recv = recv(socket, buff, MAX_BUFF_SIZE, 0);
			if (bytes_recv == SOCKET_ERROR) {
				fprintf(stderr, "Send Failed, Termination of Socket Connection\n");

				std::unique_lock<std::mutex> lck(mtx_state);
				delete client; // closeSocket + De-allocation
				client = new Client();
				grid->repaint(brickLayerList, optitrackCommunicator, clients);
				return;
			}

			// 2-3-2. Deciding dstBrick
			std::unique_lock<std::mutex> lck(mtx_state);
			Brick* dstBrick = brickLayerList->getNextDstBrick(robot, grid, ProgramState::dstBrickLayerIndex);

			// All Jobs Done: Disconnect
			if (srcBrick == NULL) {
				SOCKET socket = client->getSocket();
				int bytes_send = send(socket, Instruction(InstructionType::DCN, NULL).toString().c_str(), MAX_BUFF_SIZE, 0);
				if (bytes_send == SOCKET_ERROR) {
					fprintf(stderr, "Send Failed, Termination of Socket Connection\n");
				}
				delete client; // closeSocket + De-allocation

				client = new Client();
				grid->repaint(brickLayerList, optitrackCommunicator, clients);
				return;
			}

			std::pair<Position2D, Direction2D> dstFinalPose = brickLayerList->getFinalPose(grid, dstBrick);
			robot->markAsLift(dstBrick, dstFinalPose);
			grid->repaint(brickLayerList, optitrackCommunicator, clients);
			break;


		case RobotPhase::LIFTING: // 2-4. LIFTING: Decision of Path, Saving Keypoint, and sending Instructions (MOV, PID)
			std::unique_lock<std::mutex> lck(mtx_state);
			
			// 2-4-1. Check if Current Pose == Final Pose: Send HLT, and change phase to RLZ
			if (robot->getPose() == robot->getFinalPose()) { // Current Pose == Final Pose
				// Send HLT
				SOCKET socket = client->getSocket();
				int bytes_send = send(socket, Instruction(InstructionType::HLT, NULL).toString().c_str(), MAX_BUFF_SIZE, 0);
				if (bytes_send == SOCKET_ERROR) {
					fprintf(stderr, "Send Failed, Termination of Socket Connection\n");

					delete client; // closeSocket + De-allocation
					client = new Client();
					grid->repaint(brickLayerList, optitrackCommunicator, clients);
					return;
				}

				// Change Phase to Release
				robot->markAsRelease();
				grid->repaint(brickLayerList, optitrackCommunicator, clients);
				break;
			}

			// 2-4-2. Check if Current Pose == Keypoint: Send HLT, Send PID, Receive DONE/ERROR
			if (robot->getPose() == robot->getKeypoint()) { // Current Pose == Keypoint
				// Send HLT
				SOCKET socket = client->getSocket();
				int bytes_send = send(socket, Instruction(InstructionType::HLT, NULL).toString().c_str(), MAX_BUFF_SIZE, 0);
				if (bytes_send == SOCKET_ERROR) {
					fprintf(stderr, "Send Failed, Termination of Socket Connection\n");

					delete client; // closeSocket + De-allocation
					client = new Client();
					grid->repaint(brickLayerList, optitrackCommunicator, clients);
					return;
				}

				// Pathfinding, Save Keypoint in Robot
				std::pair<Position2D, Direction2D> pose = robot->getPose();
				std::pair<Position2D, Direction2D> keypoint = getKeyPoint(pose, robot->getFinalPose());
				robot->setKeypoint(keypoint);

				grid->repaint(brickLayerList, optitrackCommunicator, clients);

				lck.unlock();

				// Send PID
				float param[PID_PARAM] = { pose.first.x, pose.first.y, pose.second.x, pose.second.y, keypoint.first.x, keypoint.first.y, keypoint.second.x, keypoint.second.y };
				int bytes_send = send(socket, Instruction(InstructionType::PID, param).toString().c_str(), MAX_BUFF_SIZE, 0);
				if (bytes_send == SOCKET_ERROR) {
					fprintf(stderr, "Send Failed, Termination of Socket Connection\n");

					lck.lock();
					delete client; // closeSocket + De-allocation
					client = new Client();
					grid->repaint(brickLayerList, optitrackCommunicator, clients);
					return;
				}

				// Receive DONE/ERROR
				char buff[MAX_BUFF_SIZE] = { 0, };
				int bytes_recv = recv(socket, buff, MAX_BUFF_SIZE, 0);
				if (bytes_recv == SOCKET_ERROR) {
					fprintf(stderr, "Send Failed, Termination of Socket Connection\n");

					lck.lock();
					delete client; // closeSocket + De-allocation
					client = new Client();
					grid->repaint(brickLayerList, optitrackCommunicator, clients);
					return;
				}
				if (buff == "ERROR") {
					fprintf(stderr, "PID Initialization Failed\n");
				}

				break;
			}

			// 2-4-3. Send 'MOV' Instruction
			std::pair<Position2D, Direction2D> pose = robot->getPose();
			float param[PID_PARAM] = { pose.first.x, pose.first.y, pose.second.x, pose.second.y, 0, };
			int bytes_send = send(socket, Instruction(InstructionType::MOV, param).toString().c_str(), MAX_BUFF_SIZE, 0);
			if (bytes_send == SOCKET_ERROR) {
				fprintf(stderr, "Send Failed, Termination of Socket Connection\n");

				lck.lock();
				delete client; // closeSocket + De-allocation
				client = new Client();
				grid->repaint(brickLayerList, optitrackCommunicator, clients);
				return;
			}
			break;


		case RobotPhase::RELEASE: // 2-5. Release: Send 'RLZ' Instruction, and setting phase to STOP
			// 2-5-1. Send RLZ, and receive 'DONE'
			// Send RLZ
			
			float param[PID_PARAM] = {}; // TODO: Calculate (R, Z)
			int bytes_send = send(socket, Instruction(InstructionType::RLZ, param).toString().c_str(), MAX_BUFF_SIZE, 0);
			if (bytes_send == SOCKET_ERROR) {
				fprintf(stderr, "Send Failed, Termination of Socket Connection\n");

				std::unique_lock<std::mutex> lck(mtx_state);
				delete client; // closeSocket + De-allocation
				client = new Client();
				grid->repaint(brickLayerList, optitrackCommunicator, clients);
				return;
			}

			// Receive DONE/ERROR
			char buff[MAX_BUFF_SIZE] = { 0, };
			int bytes_recv = recv(socket, buff, MAX_BUFF_SIZE, 0);
			if (bytes_recv == SOCKET_ERROR) {
				fprintf(stderr, "Send Failed, Termination of Socket Connection\n");

				std::unique_lock<std::mutex> lck(mtx_state);
				delete client; // closeSocket + De-allocation
				client = new Client();
				grid->repaint(brickLayerList, optitrackCommunicator, clients);
				return;
			}
			
			// 2-5-2. Change phase to STOP
			std::unique_lock<std::mutex> lck(mtx_state);
			robot->markAsStop();

			break;
		}
	}
}

// Emergency Stop
void ProgramState::makeStop()
{
	// Lock Acquired
	std::unique_lock<std::mutex> lck(mtx_program); // Updating connQueue

	ProgramState::isTerminationActivated = true;
}