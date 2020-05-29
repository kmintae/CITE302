/**
 * Socket.h
 * Purpose: Server Socket Implementation
 * @author Mintae Kim
 */

 // Reference: https://blog.naver.com/PostView.nhn?blogId=dlghks44&logNo=221262048861&parentCategoryNo=&categoryNo=21&viewDate=&isShowPopularPosts=true&from=search
#include "Socket.h"

void clientMain()
{
    WSADATA wsaData;
    SOCKET hSocket;
    SOCKADDR_IN servAddr;

    std::string s;

    char robotConfig[MAX_BUFF_SIZE];
    char dataReceived[MAX_BUFF_SIZE];

    // WinSock Start-up for Socket Programming
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        printf("WSAStartup() errer!");

    // Socket Initialization
    hSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (hSocket == INVALID_SOCKET)
        printf("hSocketet() error!");

    // Socket Configuration
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(SERVER_IP_ADDR);
    servAddr.sin_port = htons(SERVER_PORT);

    if (connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
        printf("connect() error!");

    /// Client Flow 1 : Send Robot_Arm Number & Color Data (Optional)

    // Fetching Robot Configuration Data

    // Sending Robot Configuration Data

    /// Client Flow 2 : Iteration with While Loop, Executing action for robot arm instructions
    while (true)
    {
        recv(hSocket, dataReceived, MAX_BUFF_SIZE, 0);
        if (1) { // Move Front
            robot.robotWheels.movFront();
        }
        else if (2) { // Move Back
            robot.robotWheels.movBack();
        }
        else if (3) { // Rotate Left
            robot.robotWheels.rotLeft();
        }
        else if (4) { // Rotate Right
            robot.robotWheels.rotRight();
        }
        else if (5) { // GRAB
            robot.robotWheels.hiatus();
            robot.robotArm.grab();
            send("");
        }
    }
    send(hSocket, message, strLen, 0); // message 배열에 있는 데이터를 연결된 곳에 strLen 크기만큼 보냅니다.
    // send(hSocket, message.c_str(), message.size(), 0);
    recv(hSocket, message, 2, 0); // 연결된 곳에서 보낸 데이터를 message에 저장합니다. 메세지 길이는 2로 지정하였습니다.

    // Closing Socket
    closesocket(hSocket);
    WSACleanup();

    return;
}