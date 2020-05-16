/**
 * Socket.h
 * Purpose: Client Socket Implementation
 * @author Mintae Kim
 */

// Reference: https://blog.naver.com/PostView.nhn?blogId=dlghks44&logNo=221262048861&parentCategoryNo=&categoryNo=21&viewDate=&isShowPopularPosts=true&from=search

#ifndef CLIENT_PROCEsS_SOCKET_H_
#define CLIENT_PROCEsS_SOCKET_H_

#define SERVER_IP_ADDR "127.0.0.1"
#define SERVER_PORT 8000

#define MAX_BUFF_SIZE 60000

#include <unistd.h> 
#include <stdio.h> 
#include <sstream>
#include <string>
#include <Winsock2.h>
#include <pthread.h>
#pragma comment(lib,"ws2_32.lib")

#include "../struct/Robo.h"

// Robot Declaration: Global / Multi-Thread Sharing
struct Robot robot;

void clientMain();

#endif // CLIENT_PROCEsS_SOCKET_H_