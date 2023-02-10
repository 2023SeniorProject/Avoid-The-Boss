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
	// iocpEvent를 복원한다.
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

	if (SocketUtil::SetReuseAddress(_listenSock, true) == false) // 재사용 가능한 주소인지 확인
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

// Accept Event를 걸어줘 Iocp에서 처리할 수 있는 일감을 던져주는 역할을 수행한다.
// SocketUtil::AcceptEx를 여기서 사용할 것임.
void AcceptManager::RegisterAccept(AcceptEvent* acceptEvent)
{
	ServerSession* session = new ServerSession();

	// 나중에 어떤 세션에 관해 이를 진행했는지 알 수 있기 위해서 acceptEvent에 세션을 포함해서 넘겨주도록 한다.
	acceptEvent->Init();
	acceptEvent->_session = session;

	DWORD recvBytes(0);
	
	// 1. 리슨 소켓
	// 2. 클라 소켓
	// 3. accept시 전달되는 데이터
	// 4. 5. 원격 주소와 로컬 주소를 담기 위한 버퍼 사이즈로 SOCKADDR_IN + 16 크기로 고정됨
	bool retVal = SocketUtil::AcceptEx(_listenSock, session->_sock, acceptEvent->_buf, (BUFSIZE / 2) - 1,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, OUT &recvBytes, static_cast<LPOVERLAPPED>(acceptEvent));
	
	if (!retVal)
	{
		const int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			// 일단 다시 Accept 걸어준다.
			// accept를 실패한 경우 다시 다른 클라이언트가 연결될 수 있도록 한다.
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

// Accept 처리 완료 시 , 후처리를 진행한다. callBack 처리
void AcceptManager::ProcessAccept(AcceptEvent* acceptEvent)
{
	ServerSession* session = acceptEvent->_session; // 복원된 세션을 가져온다.
	ASSERT_CRASH(ServerIocpCore.Register(session)); // iocp핸들에 소켓 등록
	C2S_LOGIN* lp = reinterpret_cast<C2S_LOGIN*>(acceptEvent->_buf);
	
	wstring sqlExec(L"EXEC search_user_db ");
	sqlExec += lp->name;
	sqlExec += L", ";
	sqlExec += lp->pw;

	int32 sid = GetNewSessionIdx();
	session->_sid = sid;
	LoginProcess(*session, sqlExec);
	//클라이언트 소켓과 서버 리슨 소켓과 옵션을 동일하게 맞춰준다.
	
	

	if (false == SocketUtil::SetUpdateAcceptSocket(session->_sock, _listenSock))
	{
		RegisterAccept(acceptEvent);
		return;
	}

	// 클라이언트 ID 셋팅 or unordered_map 컨테이너에 담는다.
	// TODO
	
	curAcceptCnt.fetch_add(1);
	{ // 맵에다 추가하는 파트 이므로 락 걸어준다.

		WRITE_SERVER_LOCK;
		ServerIocpCore._cList.insert(sid);                 // 세션 id 추가
		ServerIocpCore._clients.try_emplace(sid, session); // 세션 추가 후
	}
	
	session->_status = USER_STATUS::LOBBY;
	session->DoRecv();  // recv 상태로 만든다.
	
	RegisterAccept(acceptEvent); // 다시 acceptEvent를 등록한다.
}

int32 AcceptManager::GetNewSessionIdx()
{

	return curAcceptCnt.load();
	
}
