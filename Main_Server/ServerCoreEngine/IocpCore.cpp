#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"
#include "Session.h"
#include "SocketUtil.h"

SIocpCore ServerIocpCore;

CIocpCore ClientIocpCore;


IocpCore::IocpCore()
{
	_hIocp = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0); // iocp 핸들 생성
	//rMgr = new RoomManager();
	//rMgr->Init();
	ASSERT_CRASH(_hIocp != INVALID_HANDLE_VALUE); 
}

IocpCore::~IocpCore()
{
	::CloseHandle(_hIocp);
	//delete rMgr;
}

// 보통은 클라이언트 소켓을 받아 이를 적용했다.

bool IocpCore::Register(IocpObject* iocpObj) 
{
    return ::CreateIoCompletionPort(iocpObj->GetHandle(), _hIocp,/*key*/reinterpret_cast<ULONG_PTR>(iocpObj), 0);
	// 나중에 가면 소켓 외에도 다양한 값들을 추가할 수 있다.
	// iocpObj->GetHandle() --> 보통은 소켓을 담는다.
	// 키값으로 자기자신을 등록한다. 포인터의 크기는 똑같으니까~
}

bool IocpCore::Processing(uint32_t time_limit) // worker thread 기능 완료된 비동기 통지 명령들을 받아와 적절하게 처리한다.
{
	DWORD numOfBytes(0); // 몇 바이트가 전송되었는가?
	IocpObject* iocpObject = nullptr; // 일감이 완료된 iocpObject의 종류를 복원하기 위한 IocpObject
	IocpEvent* iocpEvent = nullptr; // 일감이 완료된 iocpEvent의 종류(Accept인가?)
	
	BOOL retVal = ::GetQueuedCompletionStatus(_hIocp, OUT & numOfBytes, reinterpret_cast<PULONG_PTR>(&iocpObject), // 하지만 이렇게 iocpObject를 인자로 넘겨주게 되면, 다른 스레드에서 이 오브젝트를 삭제했을 때, 문제가 생길 수도 있다. --> 
		//애초에 iocpEvent에서 해당 iocp객체들에 관한 정보(해당 이벤트를 호출한 주인 iocp객체들)을 담고 있도록하자.
		OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), time_limit);
	
	if (!retVal) // 실패했다면 에러코드 확인
	{
		int32 errCode = ::WSAGetLastError();
		switch (errCode)
		{
			case WAIT_TIMEOUT: // time_limit이 INFINITE가 아닌 경우 ==> 나중에 다중 접속 시, 접속 시간에 따라 지정 가능
				std::cout << "Time Out Plz Check Your Network Condition" << std::endl;
				return false;
			default:
				// TODO : 로그 찍기
			{
				std::cout << ::WSAGetLastError() << std::endl;
				ServerSession* s = static_cast<ServerSession*>(iocpObject);
				Disconnect(s->_sid);
			}
			return false;
		}
	}

	// 클라이언트가 정상적으로 종료한 경우
	if (numOfBytes == 0 && (iocpEvent->_comp == EventType::Recv || iocpEvent->_comp == EventType::Send))
	{
		//Disconnect
		ServerSession* s = static_cast<ServerSession*>(iocpObject);
		Disconnect(s->_sid);
		if (iocpEvent->_comp == EventType::Send) delete iocpEvent;
		return false;
	}

	iocpObject->Processing(iocpEvent, numOfBytes); // 성공하면 전반적인 프로세싱을 시작해보자
	
	return true;
}

void IocpCore::Disconnect(int32 sid)
{
}

SIocpCore::SIocpCore()
{
	_rmgr = new RoomManager();
	_rmgr->Init();
}

SIocpCore::~SIocpCore()
{
	delete _rmgr;
}

void SIocpCore::Disconnect(int32 sid)
{
	if(sid >= 0 && _clients[sid]->_myRm != -1) _rmgr->ExitRoom(sid, _clients[sid]->_myRm);
	WLock;
	std::cout << "[" << _clients[sid]->_cid << "] Disconnected" << std::endl;
	_cList.erase(_clients[sid]->_cid);
	delete _clients[sid];
	_clients.erase(sid); 
}


CIocpCore::CIocpCore()
{
	_client = new ClientSession();
	::ZeroMemory(&_serveraddr, sizeof(sockaddr_in));
}

CIocpCore::~CIocpCore()
{
	if (_client != nullptr) delete _client;
}

void CIocpCore::InitConnect(const char* address)
{
	_client->_sock = SocketUtil::CreateSocket();
	_client->_sid = 0;
	ASSERT_CRASH(_client->_sock != INVALID_SOCKET);
	_serveraddr.sin_family = AF_INET;
	_serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	_serveraddr.sin_port = htons(0);
	ASSERT_CRASH(::bind(_client->_sock, reinterpret_cast<sockaddr*>(&_serveraddr), sizeof(_serveraddr)) != SOCKET_ERROR);
	inet_pton(AF_INET, address, &_serveraddr.sin_addr);
	_serveraddr.sin_port = htons(PORTNUM);
	ASSERT_CRASH(Register(static_cast<IocpObject*>(_client)));
}

void CIocpCore::DoConnect(void* loginInfo)
{
	DWORD sendBytes(0);
	DWORD sendLength = BUFSIZE / 2;
	ConnectEvent* _connectEvent = new ConnectEvent();
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

void CIocpCore::Disconnect(int32 sid = 0)
{
	std::cout << "Disconnect Client" << std::endl;
	delete _client;
	_client = nullptr;
}
