#include "pch.h"
#include "SocketUtil.h"
#include "IocpEvent.h"
#include "Session.h"
#include "AcceptManager.h"

HANDLE AcceptManager::GetHandle()
{
	return reinterpret_cast<HANDLE>(_listenSock);
}

void AcceptManager::CheckCompletionWork(IocpEvent* iocpEvent, int32 numOfBytes)
{
	ASSERT_CRASH(iocpEvent->GetType() == EventType::Accept);
	AcceptEvent* acceptEvent = static_cast<AcceptEvent*>(iocpEvent);
	ProcessAccept(acceptEvent);
}

bool AcceptManager::InitAccept()
{
	_listenSock = SocketUtil::CreateSocket();
	if (_listenSock == INVALID_SOCKET)
		return false;

	if (GIocpCore.Register(this) == false)
		return false;

	if (SocketUtil::SetReuseAddress(_listenSock, true) == false) // 재사용 가능한 주소인지 확인
		return false;

	if (SocketUtil::SetLinger(_listenSock, 0, 0) == false)
		return false;

	if (SocketUtil::Bind(_listenSock) == false)
		return false;

	if (SocketUtil::Listen(_listenSock) == false)
		return false;

	const int32 acceptCount = 1;
	for (int32 i = 0; i < acceptCount; i++)
	{
		AcceptEvent* acceptEvent = new AcceptEvent;
		_acceptEvents.push_back(acceptEvent);
		RegisterAccept(acceptEvent);
	}

	return false;
}

void AcceptManager::RegisterAccept(AcceptEvent* acceptEvent)
{
	Session* session = new Session();

	acceptEvent->Init();
	acceptEvent->SetSession(session);

	DWORD bytesReceived = 0;
	if (false == SocketUtil::AcceptEx(_listenSock, session->GetSock(), session->_recvBuf, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, OUT & bytesReceived, static_cast<LPOVERLAPPED>(acceptEvent)))
	{
		const int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			// 일단 다시 Accept 걸어준다
			RegisterAccept(acceptEvent);
		}
	}
}

void AcceptManager::ProcessAccept(AcceptEvent* acceptEvent)
{
	Session* session = acceptEvent->GetSession();

	if (false == SocketUtil::SetUpdateAcceptSocket(session->GetSock(), _listenSock))
	{
		RegisterAccept(acceptEvent);
		return;
	}

	SOCKADDR_IN sockAddress;
	int32 sizeOfSockAddr = sizeof(sockAddress);
	if (SOCKET_ERROR == ::getpeername(session->GetSock(), OUT reinterpret_cast<SOCKADDR*>(&sockAddress), &sizeOfSockAddr))
	{
		RegisterAccept(acceptEvent);
		return;
	}
	cout << "Client Connected!" << endl;

	// TODO
	RegisterAccept(acceptEvent);
}
