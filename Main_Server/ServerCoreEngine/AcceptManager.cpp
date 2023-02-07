#include "pch.h"
#include "AcceptManager.h"
#include "SocketUtil.h"

#include "OBDC_MGR.h"
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
	bool retVal = SocketUtil::AcceptEx(_listenSock, session->_sock, acceptEvent->_buf, BUFSIZE / 2,
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


void LoginProcess(GameSession& s, wstring sqlexec)
{
	USER_DB_MANAGER udb;
	udb.AllocateHandles();
	udb.ConnectDataSource(L"USER_DB");
	const WCHAR* a = sqlexec.c_str();
	udb.ExecuteStatementDirect(a);
	udb.RetrieveResult();

	wlock_guard writeLockGuard(s._locks[0]);
	{
		s._cid = udb.user_cid;
		if (s._cid == -1)
		{
			cout << "LoginFail" << endl;
			udb.DisconnectDataSource();
			s.DoSendLoginPacket(false);
			return;
		}
	}
	cout << "client[" << s._cid << "] " << "LoginSuccess" << endl;
	udb.DisconnectDataSource();
	s.DoSendLoginPacket(true);
}

// Accept ó�� �Ϸ� �� , ��ó���� �����Ѵ�. callBack ó��
void AcceptManager::ProcessAccept(AcceptEvent* acceptEvent)
{
	GameSession* session = acceptEvent->_session; // ������ ������ �����´�.
	ASSERT_CRASH(GIocpCore.Register(session)); // iocp�ڵ鿡 ���� ���
	C2S_LOGIN* lp = reinterpret_cast<C2S_LOGIN*>(acceptEvent->_buf);
	
	wstring sqlExec(L"EXEC search_user_db ");
	sqlExec += lp->name;
	sqlExec += L", ";
	sqlExec += lp->pw;
	LoginProcess(*session, sqlExec);
	//Ŭ���̾�Ʈ ���ϰ� ���� ���� ���ϰ� �ɼ��� �����ϰ� �����ش�.
	if (false == SocketUtil::SetUpdateAcceptSocket(session->_sock, _listenSock))
	{
		RegisterAccept(acceptEvent);
		return;
	}

	// Ŭ���̾�Ʈ ID ���� or unordered_map �����̳ʿ� ��´�.
	// TODO
	int32 sid = GetNewSessionIdx();
	curAcceptCnt.fetch_add(1);
	GIocpCore._clients[sid] =  session; // ���� �߰� ��

	session->_sid = sid;
	session->_status = STATUS::LOGIN;
	session->DoRecv();  // recv ���·� �����.
	RegisterAccept(acceptEvent); // �ٽ� acceptEvent�� ����Ѵ�.
}

int32 AcceptManager::GetNewSessionIdx()
{

	for (int i = 0; i < 1000; ++i)
	{
		if (GIocpCore._clients[i] == nullptr) return i;
	}
	
}
