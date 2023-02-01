#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"

IocpCore::IocpCore()
{
	_hIocp = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0); // iocp 핸들 생성
	ASSERT_CRASH(_hIocp == INVALID_HANDLE_VALUE); 
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
	IocpObject* iocpObject = nullptr; // 일감이 완료된 iocpObject의 종류를 복원하기 위한 IocpObject
	IocpEvent* iocpEvent = nullptr; // 일감이 완료된 iocpEvent의 종류(Accept인가?)
	
	BOOL retVal = ::GetQueuedCompletionStatus(_hIocp, OUT & numOfBytes, OUT reinterpret_cast<PULONG_PTR>(&iocpObject), 
		OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), time_limit);
	if (retVal) // 성공 했는가?
	{
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
				iocpObject->Processing(iocpEvent, numOfBytes);
				break;
		}
	}

	return true;
}
