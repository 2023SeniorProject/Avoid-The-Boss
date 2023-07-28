#include "pch.h"
#include "clientIocpCore.h"
#include "SocketUtil.h"
#include "IocpEvent.h"


CCIocpCore clientCore;

//=============== new Iocp Core =======================
//=====================================================
//=====================================================


CCIocpCore::CCIocpCore()
{
	_client = new CSession();
	::ZeroMemory(&_serveraddr, sizeof(sockaddr_in));
}

CCIocpCore::~CCIocpCore()
{
	if (_client != nullptr) delete _client;
}

void CCIocpCore::InitConnect(const char* address)
{
	
		_client->_sock = SocketUtil::CreateSocket();
		_client->_sid = -1;
		ASSERT_CRASH(_client->_sock != INVALID_SOCKET);
		_serveraddr.sin_family = AF_INET;
		_serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
		_serveraddr.sin_port = htons(0);
		ASSERT_CRASH(::bind(_client->_sock, reinterpret_cast<sockaddr*>(&_serveraddr), sizeof(_serveraddr)) != SOCKET_ERROR);
		inet_pton(AF_INET, address, &_serveraddr.sin_addr);
		_serveraddr.sin_port = htons(PORTNUM);
		ASSERT_CRASH(Register(static_cast<IocpObject*>(_client)));
	
	
}

void CCIocpCore::DoConnect(void* loginInfo)
{
	
		DWORD sendBytes(0);
		DWORD sendLength = BUFSIZE / 2;
		ConnectEvent* _connectEvent = new ConnectEvent();
		//memcpy(_connectEvent->_buf, loginInfo, BUFSIZE / 2);
		bool retVal = SocketUtil::ConnectEx(_client->_sock, reinterpret_cast<sockaddr*>(&_serveraddr), sizeof(_serveraddr), _connectEvent->_buf, (BUFSIZE / 2) - 1, NULL,
			static_cast<LPWSAOVERLAPPED>(_connectEvent));

		if (!retVal)
		{
			const int32 errorCode = ::WSAGetLastError();
			if (errorCode == WSAETIMEDOUT)
			{
				std::cout << "Time Out\n";
				DoConnect(nullptr);
			}
			if (errorCode != WSA_IO_PENDING)
			{
				std::cout << errorCode << std::endl;
				std::cout << "Connect Error" << std::endl;
				delete _client;
				SocketUtil::Clear();
			}
			
		}
	
}

bool CCIocpCore::Processing(uint32_t timelimit)
{
		DWORD numOfBytes(0); // 몇 바이트가 전송되었는가?
		IocpObject* iocpObject = nullptr; // 일감이 완료된 iocpObject의 종류를 복원하기 위한 IocpObject
		IocpEvent* iocpEvent = nullptr; // 일감이 완료된 iocpEvent의 종류(Accept인가?)

		BOOL retVal = ::GetQueuedCompletionStatus(_hIocp, OUT & numOfBytes, reinterpret_cast<PULONG_PTR>(&iocpObject), // 하지만 이렇게 iocpObject를 인자로 넘겨주게 되면, 다른 스레드에서 이 오브젝트를 삭제했을 때, 문제가 생길 수도 있다. --> 
			//애초에 iocpEvent에서 해당 iocp객체들에 관한 정보(해당 이벤트를 호출한 주인 iocp객체들)을 담고 있도록하자.
			OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), timelimit);

		if (!retVal) // 실패했다면 에러코드 확인
		{
			int32 errCode = ::WSAGetLastError();
			switch (errCode)
			{
			case ERROR_CONNECTION_REFUSED:
			case WSAECONNREFUSED:
				if (_client)
				{
					std::cout << "Check The Server On... Retry Connecting\n";
					DoConnect(nullptr);
					return true;
				}
				else return false;
				break;
			case WAIT_TIMEOUT: // time_limit이 INFINITE가 아닌 경우 ==> 나중에 다중 접속 시, 접속 시간에 따라 지정 가능
				return false;
			default:
				// TODO : 로그 찍기
			{
				std::cout << ::WSAGetLastError() << "\n";
				ASession* s = static_cast<ASession*>(iocpObject);
				Disconnect(s->_sid);
			}
			return false;
			}
		}

		// 클라이언트가 정상적으로 종료한 경우
		if (numOfBytes == 0 && (iocpEvent->_comp == EventType::Recv || iocpEvent->_comp == EventType::Send))
		{
			//Disconnect
			ASession* s = static_cast<ASession*>(iocpObject);
			Disconnect(s->_sid);
			if (iocpEvent->_comp == EventType::Send) delete iocpEvent;
			return false;
		}
		iocpObject->Processing(iocpEvent, numOfBytes); // 성공하면 전반적인 프로세싱을 시작해보자;
		return true;

}

void CCIocpCore::Disconnect(int32 sid = 0)
{
	std::cout << "Disconnect Client" << std::endl;
	delete _client;
	_client = nullptr;
}