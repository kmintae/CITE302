/**
 * Socket.h
 * Purpose: Server Socket Implementation
 * @author Mintae Kim
 * @author chrisysl (https://kevinthegrey.tistory.com/26)
 * @author Aaron Anderson (https://www.daniweb.com/programming/software-development/threads/6811/winsock-multi-client-servers)
 */

#include "Socket.h"

void serverSocket()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET hListen;
	hListen = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	int port;
	port = GetPrivateProfileInt("connection", "SERVER_PORT", -1, "../../../config/server.ini");
	if (port == -1) {
		fprintf(stderr, "config/server.ini Not Existing. Terminating...\n");
		return;
	}

	SOCKADDR_IN tListenAddr = {};
	tListenAddr.sin_family = AF_INET;
	tListenAddr.sin_port = htons(port);
	tListenAddr.sin_addr = htonl(INADDR_ANY);

	// Allowing Reuse of Socket Resources
	int i = 1;
	setsockopt(hListen, SOL_SOCKET, SO_REUSEADDR, (char*)&i, sizeof(i));

	bind(hListen, (SOCKADDR*)&tListenAddr, sizeof(tListenAddr));

	int maxRobotLimit;
	maxRobotLimit = GetPrivateProfileInt("connection", "MAX_ROBOT_CONNECTED", 2, "../../../config/server.ini");

	listen(hListen, maxRobotLimit);

	SOCKADDR_IN tClntAddr = {};
	int iClntSize = sizeof(tClntAddr);
	SOCKET hClient = accept(hListen, (SOCKADDR*)&tClntAddr, &iClntSize);

	// Using As Blocking Mode: Multi-Threading

	while (true)
	{

	}

	closesocket(hListen);

	WSACleanup();
	return;
}