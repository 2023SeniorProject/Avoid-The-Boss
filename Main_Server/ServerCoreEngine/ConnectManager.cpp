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

	// ���߿� � ���ǿ� ���� �̸� �����ߴ��� �� �� �ֱ� ���ؼ� acceptEvent�� ������ �����ؼ� �Ѱ��ֵ��� �Ѵ�.
	

	DWORD sendBytes(0);
	DWORD sendLength = BUFSIZE / 2;
	// 1. ���� ����
	// 2. Ŭ�� ����
	// 3. accept�� ���޵Ǵ� ������
	// 4. 5. ���� �ּҿ� ���� �ּҸ� ��� ���� ���� ������� SOCKADDR_IN + 16 ũ��� ������
	ConnectEvent* _connectEvent = new ConnectEvent();
	memcpy(_connectEvent->_buf, idPw, sendLength);
	bool retVal = SocketUtil::ConnectEx(_sock, reinterpret_cast<sockaddr*>(&_serveraddr),sizeof(_serveraddr),_connectEvent->_buf,sendLength,&sendBytes,
		static_cast<LPWSAOVERLAPPED>(_connectEvent));

	if (!retVal)
	{
		const int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			// �ϴ� �ٽ� Accept �ɾ��ش�.
			//  connect�� ������ ��� Ŭ���̾�Ʈ ����.
			std::cout << errorCode << std::endl;
			std::cout << "Connect Error" << std::endl;
			//exit(1); //  
		}
	}
	cout << sendBytes << endl;
}
