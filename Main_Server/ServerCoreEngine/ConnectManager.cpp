#include "pch.h"
#include "Session.h"
#include "SocketUtil.h"
#include "IocpEvent.h"
#include "ConnectManager.h"

HANDLE ConnectManager::GetHandle()
{
    return reinterpret_cast<HANDLE>(_sock);
}

void ConnectManager::Processing(IocpEvent* iocpEvent, int32 numOfBytes)
{
	ASSERT_CRASH(iocpEvent->_comp == EventType::Connect);
	ConnectEvent* connectEvent = static_cast<ConnectEvent*>(iocpEvent);
	std::cout << "ConnectEx Successs " << std::endl;
}

void ConnectManager::InitConnect(GameSession& csession , const char* address)
{
	csession._sock = SocketUtil::CreateSocket();
	_sock = csession._sock;
	ASSERT_CRASH(csession._sock != INVALID_SOCKET);
	_serveraddr.sin_family = AF_INET;
	_serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	_serveraddr.sin_port = htons(0);
	ASSERT_CRASH(::bind(_sock, reinterpret_cast<sockaddr*>(&_serveraddr), sizeof(_serveraddr)) != SOCKET_ERROR);
	inet_pton(AF_INET,address,&_serveraddr.sin_addr);
	_serveraddr.sin_port = htons(PORTNUM);
	ASSERT_CRASH(ClientIocpCore.Register(this));
}

void ConnectManager::DoConnect(const char* idPw)
{

	// 나중에 어떤 세션에 관해 이를 진행했는지 알 수 있기 위해서 acceptEvent에 세션을 포함해서 넘겨주도록 한다.
	

	DWORD sendBytes(0);
	DWORD sendLength = BUFSIZE / 2;
	// 1. 리슨 소켓
	// 2. 클라 소켓
	// 3. accept시 전달되는 데이터
	// 4. 5. 원격 주소와 로컬 주소를 담기 위한 버퍼 사이즈로 SOCKADDR_IN + 16 크기로 고정됨
	ConnectEvent* _connectEvent = new ConnectEvent();
	memcpy(_connectEvent->_buf, idPw, sendLength);
	bool retVal = SocketUtil::ConnectEx(_sock, reinterpret_cast<sockaddr*>(&_serveraddr),sizeof(_serveraddr),_connectEvent->_buf,sendLength,&sendBytes,
		static_cast<LPWSAOVERLAPPED>(_connectEvent));

	if (!retVal)
	{
		const int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			// 일단 다시 Accept 걸어준다.
			//  connect를 실패한 경우 클라이언트 종료.
			std::cout << errorCode << std::endl;
			std::cout << "Connect Error" << std::endl;
			//exit(1); //  
		}
	}
	cout << sendBytes << endl;
}
