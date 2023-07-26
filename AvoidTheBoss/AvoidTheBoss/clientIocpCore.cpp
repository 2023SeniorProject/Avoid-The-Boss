#include "pch.h"
#include "clientIocpCore.h"
#include "SocketUtil.h"
#include "IocpEvent.h"


CCIocpCore clientCore;

//=============== new Iocp Core =======================
//=====================================================
//=====================================================


CCIocpCore::CCIocpCore()
{
	_client = new CSession();
	::ZeroMemory(&_serveraddr, sizeof(sockaddr_in));
}

CCIocpCore::~CCIocpCore()
{
	if (_client != nullptr) delete _client;
}

void CCIocpCore::InitConnect(const char* address)
{
	
		_client->_sock = SocketUtil::CreateSocket();
		_client->_sid = -1;
		ASSERT_CRASH(_client->_sock != INVALID_SOCKET);
		_serveraddr.sin_family = AF_INET;
		_serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
		_serveraddr.sin_port = htons(0);
		ASSERT_CRASH(::bind(_client->_sock, reinterpret_cast<sockaddr*>(&_serveraddr), sizeof(_serveraddr)) != SOCKET_ERROR);
		inet_pton(AF_INET, address, &_serveraddr.sin_addr);
		_serveraddr.sin_port = htons(PORTNUM);
		ASSERT_CRASH(Register(static_cast<IocpObject*>(_client)));
	
	
}

void CCIocpCore::DoConnect(void* loginInfo)
{
	
		DWORD sendBytes(0);
		DWORD sendLength = BUFSIZE / 2;
		ConnectEvent* _connectEvent = new ConnectEvent();
		//memcpy(_connectEvent->_buf, loginInfo, BUFSIZE / 2);
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

void CCIocpCore::Disconnect(int32 sid = 0)
{
	std::cout << "Disconnect Client" << std::endl;
	delete _client;
	_client = nullptr;
}