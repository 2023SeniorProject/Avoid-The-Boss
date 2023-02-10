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
			// �ϴ� �ٽ� Accept �ɾ��ش�.
			// accept�� ������ ��� �ٽ� �ٸ� Ŭ���̾�Ʈ�� ����� �� �ֵ��� �Ѵ�.
			std::cout << "Accept Error" << std::endl;
			RegisterAccept(acceptEvent);
		}
	}

}


void LoginProcess(ServerSession& s, wstring sqlexec)
{
	USER_DB_MANAGER udb;
	udb.AllocateHandles();
	udb.ConnectDataSource(L"2023SENIORPROJECT");
	const WCHAR* a = sqlexec.c_str();
	udb.ExecuteStatementDirect(a);
	udb.RetrieveResult();

	std::unique_lock<std::shared_mutex> wr(s._lock);
	{
		s._cid = udb.user_cid;
		auto i = ServerIocpCore._cList.find(s._cid);
		if (s._cid == -1 ||  i != ServerIocpCore._cList.end())
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
	ServerSession* session = acceptEvent->_session; // ������ ������ �����´�.
	ASSERT_CRASH(ServerIocpCore.Register(session)); // iocp�ڵ鿡 ���� ���
	C2S_LOGIN* lp = reinterpret_cast<C2S_LOGIN*>(acceptEvent->_buf);
	
	wstring sqlExec(L"EXEC search_user_db ");
	sqlExec += lp->name;
	sqlExec += L", ";
	sqlExec += lp->pw;

	int32 sid = GetNewSessionIdx();
	session->_sid = sid;
	LoginProcess(*session, sqlExec);
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
		ServerIocpCore._cList.insert(sid);                 // ���� id �߰�
		ServerIocpCore._clients.try_emplace(sid, session); // ���� �߰� ��
	}
	
	session->_status = USER_STATUS::LOBBY;
	session->DoRecv();  // recv ���·� �����.
	
	RegisterAccept(acceptEvent); // �ٽ� acceptEvent�� ����Ѵ�.
}

int32 AcceptManager::GetNewSessionIdx()
{

	return curAcceptCnt.load();
	
}
