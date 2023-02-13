#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"
#include "Session.h"
#include "SocketUtil.h"

SIocpCore ServerIocpCore;

CIocpCore ClientIocpCore;


IocpCore::IocpCore()
{
	_hIocp = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0); // iocp �ڵ� ����
	//rMgr = new RoomManager();
	//rMgr->Init();
	ASSERT_CRASH(_hIocp != INVALID_HANDLE_VALUE); 
}

IocpCore::~IocpCore()
{
	::CloseHandle(_hIocp);
	//delete rMgr;
}

// ������ Ŭ���̾�Ʈ ������ �޾� �̸� �����ߴ�.

bool IocpCore::Register(IocpObject* iocpObj) 
{
    return ::CreateIoCompletionPort(iocpObj->GetHandle(), _hIocp,/*key*/reinterpret_cast<ULONG_PTR>(iocpObj), 0);
	// ���߿� ���� ���� �ܿ��� �پ��� ������ �߰��� �� �ִ�.
	// iocpObj->GetHandle() --> ������ ������ ��´�.
	// Ű������ �ڱ��ڽ��� ����Ѵ�. �������� ũ��� �Ȱ����ϱ�~
}

bool IocpCore::Processing(uint32_t time_limit) // worker thread ��� �Ϸ�� �񵿱� ���� ��ɵ��� �޾ƿ� �����ϰ� ó���Ѵ�.
{
	DWORD numOfBytes(0); // �� ����Ʈ�� ���۵Ǿ��°�?
	IocpObject* iocpObject = nullptr; // �ϰ��� �Ϸ�� iocpObject�� ������ �����ϱ� ���� IocpObject
	IocpEvent* iocpEvent = nullptr; // �ϰ��� �Ϸ�� iocpEvent�� ����(Accept�ΰ�?)
	
	BOOL retVal = ::GetQueuedCompletionStatus(_hIocp, OUT & numOfBytes, reinterpret_cast<PULONG_PTR>(&iocpObject), // ������ �̷��� iocpObject�� ���ڷ� �Ѱ��ְ� �Ǹ�, �ٸ� �����忡�� �� ������Ʈ�� �������� ��, ������ ���� ���� �ִ�. --> 
		//���ʿ� iocpEvent���� �ش� iocp��ü�鿡 ���� ����(�ش� �̺�Ʈ�� ȣ���� ���� iocp��ü��)�� ��� �ֵ�������.
		OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), time_limit);
	
	if (!retVal) // �����ߴٸ� �����ڵ� Ȯ��
	{
		int32 errCode = ::WSAGetLastError();
		switch (errCode)
		{
			case WAIT_TIMEOUT: // time_limit�� INFINITE�� �ƴ� ��� ==> ���߿� ���� ���� ��, ���� �ð��� ���� ���� ����
				std::cout << "Time Out Plz Check Your Network Condition" << std::endl;
				return false;
			default:
				// TODO : �α� ���
			{
				std::cout << ::WSAGetLastError() << std::endl;
				ServerSession* s = static_cast<ServerSession*>(iocpObject);
				Disconnect(s->_sid);
			}
			return false;
		}
	}

	// Ŭ���̾�Ʈ�� ���������� ������ ���
	if (numOfBytes == 0 && (iocpEvent->_comp == EventType::Recv || iocpEvent->_comp == EventType::Send))
	{
		//Disconnect
		ServerSession* s = static_cast<ServerSession*>(iocpObject);
		Disconnect(s->_sid);
		if (iocpEvent->_comp == EventType::Send) delete iocpEvent;
		return false;
	}

	iocpObject->Processing(iocpEvent, numOfBytes); // �����ϸ� �������� ���μ����� �����غ���
	
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
