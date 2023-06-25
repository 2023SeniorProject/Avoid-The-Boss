#include "pch.h"
#include "ConnectManager.h"

ConnectManager::~ConnectManager()
{
}

bool ConnectManager::BindAddress(char* address)
{
	DummySession* client = new DummySession();
	_serveraddr.sin_family = AF_INET;
	_serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	_serveraddr.sin_port = htons(0);
	ASSERT_CRASH(::bind(client->_sock, reinterpret_cast<sockaddr*>(&_serveraddr), sizeof(_serveraddr)) != SOCKET_ERROR);
	inet_pton(AF_INET, address, &_serveraddr.sin_addr);
	_serveraddr.sin_port = htons(PORTNUM);
	ASSERT_CRASH(RegisterConnect(static_cast<IocpObject*>(client)));
}

bool ConnectManager::InitConnect()
{
	return false;
}



void ConnectManager::RegisterConnect(ConnectEvent* acceptEvent)
{
	DWORD sendBytes(0);
	DWORD sendLength = BUFSIZE / 2;
	DummyConnectEvent* _connectEvent = new DummyConnectEvent();
	memcpy(_connectEvent->_buf, loginInfo, BUFSIZE / 2);
	bool retVal = SocketUtil::ConnectEx(_client->_sock, reinterpret_cast<sockaddr*>(&_serveraddr), sizeof(_serveraddr), _connectEvent->_buf, (BUFSIZE / 2) - 1, NULL,
		static_cast<LPWSAOVERLAPPED>(_connectEvent));

	if (!retVal)
	{
		const int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			std::cout << errorCode << std::endl;
			std::cout << "Connect Error" << std::endl;
			delete _client;
			SocketUtil::Clear();
		}
	}
}

void ConnectManager::ProcessConnect(ConnectEvent* acceptEvent)
{
}

int32 ConnectManager::GetNewSessionIdx()
{
	return int32();
}


void ConnectManager::Processing(IocpEvent* iocpEvent, int32 numOfBytes)
{
}


