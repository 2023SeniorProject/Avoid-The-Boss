#include "pch.h"

#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include "../FINALTERMSERVER/protocol.h"

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

#define PORTNUM 9000
#define BUFSIZE 512

using namespace std;
int main()
{
#pragma region winsock init
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 0;
	SOCKET c_sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (c_sock == INVALID_SOCKET) return 0;

	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(PORTNUM);
	inet_pton(AF_INET, "127.0.0.1", &serveraddr.sin_addr);

	WSAConnect(c_sock,(sockaddr*)&serveraddr,sizeof(serveraddr),NULL,NULL,NULL,NULL);
#pragma endregion
	WSABUF wsabuf;
	char buf[BUFSIZE];
	wsabuf.buf = buf;
	wsabuf.len = 512;
	while (true)
	{
		// TO-DO
	}



	closesocket(c_sock);
	WSACleanup();
}