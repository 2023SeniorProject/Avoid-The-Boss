#include "pch.h"
#include "AcceptManager.h"
#include "SocketUtil.h"

#include "IocpEvent.h"
#include "Session.h"


AcceptManager::~AcceptManager()
{
	SocketUtil::Close(_listenSock);
	for (auto i : _acceptEvents)
	{
		delete(i);
	}
}

HANDLE AcceptManager::GetHandle()
{
	return reinterpret_cast<HANDLE>(_listenSock);
}

void AcceptManager::Processing(IocpEvent* iocpEvent, int32 numOfBytes)
{
	// iocpEvent�� �����Ѵ�.
	ASSERT_CRASH(iocpEvent->_comp == EventType::Accept);
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

	if (SocketUtil::SetReuseAddress(_listenSock, true) == false) // ���� ������ �ּ����� Ȯ��
		return false;

	if (SocketUtil::SetLinger(_listenSock, 0, 0) == false)
		return false;

	if (SocketUtil::Bind(_listenSock) == false)
		return false;

	if (SocketUtil::Listen(_listenSock) == false)
		return false;

	for (int32 i = 0; i < maxAcceptCnt; i++)
	{
		AcceptEvent* acceptEvent = new AcceptEvent;
		_acceptEvents.push_back(acceptEvent);
		RegisterAccept(acceptEvent);
	}

	return false;
}

void AcceptManager::CloseSocket()
{
	SocketUtil::Close(_listenSock);
}

// Accept Event�� �ɾ��� Iocp���� ó���� �� �ִ� �ϰ��� �����ִ� ������ �����Ѵ�.
// SocketUtil::AcceptEx�� ���⼭ ����� ����.
void AcceptManager::RegisterAccept(AcceptEvent* acceptEvent)
{
	GameSession* session = new GameSession();

	// ���߿� � ���ǿ� ���� �̸� �����ߴ��� �� �� �ֱ� ���ؼ� acceptEvent�� ������ �����ؼ� �Ѱ��ֵ��� �Ѵ�.
	acceptEvent->Init();
	acceptEvent->_session = session;

	DWORD recvBytes(0);
	
	// 1. ���� ����
	// 2. Ŭ�� ����
	// 3. accept�� ���޵Ǵ� ������
	// 4. 5. ���� �ּҿ� ���� �ּҸ� ��� ���� ���� ������� SOCKADDR_IN + 16 ũ��� ������
	bool retVal = SocketUtil::AcceptEx(_listenSock, session->_sock, acceptEvent->_buf, 0,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, OUT &recvBytes, static_cast<LPOVERLAPPED>(acceptEvent));
	
	if (!retVal)
	{
		const int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			// �ϴ� �ٽ� Accept �ɾ��ش�.
			// accept�� ������ ��� �ٽ� �ٸ� Ŭ���̾�Ʈ�� ����� �� �ֵ��� �Ѵ�.
			std::cout << "Accept Error" << std::endl;
			RegisterAccept(acceptEvent);
		}
	}

}

// Accept ó�� �Ϸ� �� , ��ó���� �����Ѵ�. callBack ó��
void AcceptManager::ProcessAccept(AcceptEvent* acceptEvent)
{
	GameSession* session = acceptEvent->_session; // ������ ������ �����´�.

	//Ŭ���̾�Ʈ ���ϰ� ���� ���� ���ϰ� �ɼ��� �����ϰ� �����ش�.
	if (false == SocketUtil::SetUpdateAcceptSocket(session->_sock, _listenSock))
	{
		RegisterAccept(acceptEvent);
		return;
	}

	
	cout << "Client Connected!" << endl;
	// Ŭ���̾�Ʈ ID ���� or unordered_map �����̳ʿ� ��´�.
	// TODO
	int32 sid = GetNewSessionIdx();
	session->_sid = sid;
	GIocpCore. _clients.try_emplace(sid, session); // ���� �߰� ��
	GIocpCore._clients[sid]->DoRecv();  // recv ���·� �����.
	std::cout << GIocpCore._clients[sid]->_sid << std::endl;
	RegisterAccept(acceptEvent); // �ٽ� acceptEvent�� ����Ѵ�.
}

int32 AcceptManager::GetNewSessionIdx()
{
	return curAcceptCnt.load();
}
