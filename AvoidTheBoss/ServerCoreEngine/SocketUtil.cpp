#include "pch.h"
#include "SocketUtil.h"

LPFN_CONNECTEX		SocketUtil::ConnectEx = nullptr;
LPFN_DISCONNECTEX	SocketUtil::DisconnectEx = nullptr;
LPFN_ACCEPTEX		SocketUtil::AcceptEx = nullptr;

void SocketUtil::Init()
{
	WSADATA wsa;
	ASSERT_CRASH(WSAStartup(MAKEWORD(2, 2), &wsa) == 0);
	SOCKET dummySocket = CreateSocket();
	ASSERT_CRASH(BindWindowsFunction(dummySocket, WSAID_CONNECTEX, reinterpret_cast<LPVOID*>(&ConnectEx)));
	ASSERT_CRASH(BindWindowsFunction(dummySocket, WSAID_DISCONNECTEX, reinterpret_cast<LPVOID*>(&DisconnectEx)));
	ASSERT_CRASH(BindWindowsFunction(dummySocket, WSAID_ACCEPTEX, reinterpret_cast<LPVOID*>(&AcceptEx)));
	Close(dummySocket);
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

bool SocketUtil::Connect(SOCKET s, std::string Address)
{
	sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	::inet_pton(AF_INET, Address.c_str(), &serveraddr.sin_addr);
	serveraddr.sin_port = htons(PORTNUM);

	return (::WSAConnect(s, (sockaddr*)&serveraddr, sizeof(serveraddr),NULL,NULL,NULL,NULL) != SOCKET_ERROR);
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

bool SocketUtil::BindWindowsFunction(SOCKET socket, GUID guid, LPVOID* fn)
{
	DWORD bytes = 0;
	return SOCKET_ERROR != ::WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), fn, sizeof(*fn), OUT & bytes, NULL, NULL);
}

bool SocketUtil::SetLinger(SOCKET socket, uint16 onoff, uint16 linger)
{
	LINGER option;
	option.l_onoff = onoff;
	option.l_linger = linger;
	return SetSockOpt(socket, SOL_SOCKET, SO_LINGER, option);
}

bool SocketUtil::SetReuseAddress(SOCKET socket, bool flag)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_REUSEADDR, flag);
}

bool SocketUtil::SetRecvBufferSize(SOCKET socket, int32 size)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_RCVBUF, size);
}

bool SocketUtil::SetSendBufferSize(SOCKET socket, int32 size)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_SNDBUF, size);
}

bool SocketUtil::SetTcpNoDelay(SOCKET socket, bool flag)
{
	return SetSockOpt(socket, SOL_SOCKET, TCP_NODELAY, flag);
}

// ListenSocket의 특성을 ClientSocket에 그대로 적용
bool SocketUtil::SetUpdateAcceptSocket(SOCKET socket, SOCKET listenSocket)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, listenSocket);
}