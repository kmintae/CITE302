/**
 * OptitrackCommunicate.h
 * Purpose: Optitrack Communication via UDP Protocol
 * @author Mintae Kim
 */

#pragma once

#include <Windows.h>
#include <WinSock2.h>
#include <stdio.h>
#include <string>

#define MAX_UDP_BUFF_SIZE 512

void UDPSocket(SOCKET& udpSocket);

std::string fetchOptitrackData(SOCKET& udpSocket);