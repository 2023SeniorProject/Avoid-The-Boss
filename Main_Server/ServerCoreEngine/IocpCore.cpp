#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"

IocpCore GIocpCore;

IocpCore::IocpCore()
{
	_hIocp = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0); // iocp 핸들 생성
	ASSERT_CRASH(_hIocp != INVALID_HANDLE_VALUE); 
}

IocpCore::~IocpCore()
{
	::CloseHandle(_hIocp);
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
	//IocpObject* iocpObject = nullptr; // 일감이 완료된 iocpObject의 종류를 복원하기 위한 IocpObject
	IocpEvent* iocpEvent = nullptr; // 일감이 완료된 iocpEvent의 종류(Accept인가?)
	ULONG_PTR key(0);
	BOOL retVal = ::GetQueuedCompletionStatus(_hIocp, OUT & numOfBytes, 
		/*key*/&key /*&iocpObject*/, // 하지만 이렇게 iocpObject를 인자로 넘겨주게 되면, 다른 스레드에서 이 오브젝트를 삭제했을 때, 문제가 생길 수도 있다. --> 
		//애초에 iocpEvent에서 해당 iocp객체들에 관한 정보(해당 이벤트를 호출한 주인 iocp객체들)을 담고 있도록하자.
		OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), time_limit);
	if (retVal) // 성공 했는가?
	{
		IocpObjRef iocpObject = iocpEvent->_ev_owner;// 여기서 어떤 iocpObject(세션, 리스너)가 수행한 것인지 복원 진행
		iocpObject->Processing(iocpEvent, numOfBytes); // 성공하면 전반적인 프로세싱을 시작해보자
	}
	else // 실패했다면 에러코드 확인
	{
		int32 errCode = ::WSAGetLastError();
		switch (errCode)
		{
			case WAIT_TIMEOUT: // time_limit이 INFINITE가 아닌 경우 ==> 나중에 다중 접속 시, 접속 시간에 따라 지정 가능
				return false;
			default:
				// TODO : 로그 찍기
			{
				IocpObjRef iocpObject = iocpEvent->_ev_owner;
				iocpObject->Processing(iocpEvent, numOfBytes);
				break;
			}
		}
	}

	return true;
}
