#include "pch.h"



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
	int32 retval =  WSAConnect(c_sock,(sockaddr*)&serveraddr,sizeof(serveraddr),NULL,NULL,NULL,NULL);
	if (retval == SOCKET_ERROR) return -1;
#pragma endregion
	
	while (true)
	{

	}

	closesocket(c_sock);
	WSACleanup();
}