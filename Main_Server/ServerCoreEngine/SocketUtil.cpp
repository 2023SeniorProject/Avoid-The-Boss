#include "pch.h"
#include "SocketUtil.h"


void SocketUtil::Init()
{
	WSADATA wsa;
	ASSERT_CRASH(WSAStartup(MAKEWORD(2, 2), &wsa) == 0);
}

void SocketUtil::Clear()
{
	WSACleanup();
}

SOCKET SocketUtil::CreateSocket()
{
	return ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
}

bool SocketUtil::Connect(SOCKET s)
{
	return false;
}

bool SocketUtil::Bind(SOCKET s)
{
	sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(PORTNUM);

	return (::bind(s,(sockaddr*)&serveraddr,sizeof(serveraddr)) != SOCKET_ERROR);
}

bool SocketUtil::Listen(SOCKET s)
{
	return (listen(s, SOMAXCONN) != SOCKET_ERROR);
}

bool SocketUtil::Close(SOCKET& s)
{
	if(s != INVALID_SOCKET)
		closesocket(s);
	s = INVALID_SOCKET;
	return true;
}
