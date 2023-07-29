#include "pch.h"
#include "AcceptManager.h"
#include "SocketUtil.h"
#include "OBDC_MGR.h"

#include "CSIocpCore.h"
#include "Session.h"
#include "IocpEvent.h"

using namespace std;

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

	if (ServerIocpCore.Register(this) == false)
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
	ServerSession* session = new ServerSession();

	// ���߿� � ���ǿ� ���� �̸� �����ߴ��� �� �� �ֱ� ���ؼ� acceptEvent�� ������ �����ؼ� �Ѱ��ֵ��� �Ѵ�.
	acceptEvent->Init();
	acceptEvent->_session = session;

	DWORD recvBytes(0);
	
	// 1. ���� ����
	// 2. Ŭ�� ����
	// 3. accept�� ���޵Ǵ� ������
	// 4. 5. ���� �ּҿ� ���� �ּҸ� ��� ���� ���� ������� SOCKADDR_IN + 16 ũ��� ������
	bool retVal = SocketUtil::AcceptEx(_listenSock, session->_sock, acceptEvent->_buf, (BUFSIZE / 2) - 1,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, OUT &recvBytes, static_cast<LPOVERLAPPED>(acceptEvent));
	
	if (!retVal)
	{
		const int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			delete session;
			// �ϴ� �ٽ� Accept �ɾ��ش�.
			// accept�� ������ ��� �ٽ� �ٸ� Ŭ���̾�Ʈ�� ����� �� �ֵ��� �Ѵ�.
			acceptEvent->_session = nullptr;
			std::cout << "Accept Error" << std::endl;
			RegisterAccept(acceptEvent);
		}
	}

}




// Accept ó�� �Ϸ� �� , ��ó���� �����Ѵ�. callBack ó��
void AcceptManager::ProcessAccept(AcceptEvent* acceptEvent)
{
	ServerSession* session = acceptEvent->_session; // ������ ������ �����´�.
	ASSERT_CRASH(ServerIocpCore.Register(session)); // iocp�ڵ鿡 ���� ���
	C2S_LOGIN* lp = reinterpret_cast<C2S_LOGIN*>(acceptEvent->_buf);
	
	
	

	int32 sid = GetNewSessionIdx();
	session->_sid = sid;
	session->_cid = sid;
	
	
	//Ŭ���̾�Ʈ ���ϰ� ���� ���� ���ϰ� �ɼ��� �����ϰ� �����ش�.
	
	if (false == SocketUtil::SetUpdateAcceptSocket(session->_sock, _listenSock))
	{
		RegisterAccept(acceptEvent);
		return;
	}

	// Ŭ���̾�Ʈ ID ���� or unordered_map �����̳ʿ� ��´�.
	// TODO
	
	curAcceptCnt.fetch_add(1);
	{ // �ʿ��� �߰��ϴ� ��Ʈ �̹Ƿ� �� �ɾ��ش�.

		WRITE_SERVER_LOCK;
		ServerIocpCore._cList.insert(sid);                 
		ServerIocpCore._clients.try_emplace(sid, session); 
		std::cout << sid << " Accept Success\n";
	}
	
	
	S2C_ROOM_LIST packet;
	packet.size = sizeof(S2C_ROOM_LIST);
	packet.type = (uint8)S_ROOM_PACKET_TYPE::UPDATE_LIST;
	for (int32 i = 0; i < 5; ++i)
	{
		packet.member = ServerIocpCore._rmgr->GetRoom(session->_curPage * 5 + i)._memCnt.load();
		packet.rmNum = session->_curPage * 5 + i;
		session->DoSend(&packet);
	}
	session->DoRecv();  // recv ���·� �����.
	acceptEvent->_session = nullptr;
	RegisterAccept(acceptEvent); // �ٽ� acceptEvent�� ����Ѵ�.
}

int32 AcceptManager::GetNewSessionIdx()
{

	return curAcceptCnt.load();
	
}
