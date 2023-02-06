#include "pch.h"

#include "SocketUtil.h"
#include "IocpEvent.h"
#include "ClientManager.h"


void ClientManager::InitConnect(const char* address)
{
	_clientSession._sock = SocketUtil::CreateSocket();
	ASSERT_CRASH(_clientSession._sock != INVALID_SOCKET);
	_serveraddr.sin_family = AF_INET;
	_serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	_serveraddr.sin_port = htons(0);
	ASSERT_CRASH(::bind(_clientSession._sock, reinterpret_cast<sockaddr*>(&_serveraddr), sizeof(_serveraddr)) != SOCKET_ERROR);
	inet_pton(AF_INET,address,&_serveraddr.sin_addr);
	_serveraddr.sin_port = htons(PORTNUM);
	ASSERT_CRASH(ClientIocpCore.Register(&_clientSession));
}

void ClientManager::DoConnect(const char* idPw)
{
	DWORD sendBytes(0);
	DWORD sendLength = BUFSIZE / 2;
	ConnectEvent* _connectEvent = new ConnectEvent();
	memcpy(_connectEvent->_buf, idPw, BUFSIZE / 2);
	bool retVal = SocketUtil::ConnectEx(_clientSession._sock, reinterpret_cast<sockaddr*>(&_serveraddr),sizeof(_serveraddr), _connectEvent->_buf, BUFSIZE / 2 ,NULL,
		static_cast<LPWSAOVERLAPPED>(_connectEvent));

	if (!retVal)
	{
		const int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			std::cout << errorCode << std::endl;
			std::cout << "Connect Error" << std::endl;
		}
	}
}

void ClientManager::DoSend(void* packet)
{
	cout << "send" << endl;
	_clientSession.DoSend(packet);
}
