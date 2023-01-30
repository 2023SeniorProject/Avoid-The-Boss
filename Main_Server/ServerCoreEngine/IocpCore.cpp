#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"

IocpCore::IocpCore()
{
}

IocpCore::~IocpCore()
{
}

bool IocpCore::Register(IocpObject* iocpObj)
{
    return ::CreateIoCompletionPort(iocpObj->GetHandle(), _hIocp, reinterpret_cast<ULONG_PTR>(iocpObj), 0);
}

bool IocpCore::CheckCompletionWork(uint32_t time_limit) // worker thread 기능 완료된 비동기 통지 명령들을 받아와 적절하게 처리한다.
{
	DWORD numOfBytes = 0;
	IocpObject* iocpObject = nullptr;
	IocpEvent* iocpEvent = nullptr;

	if (::GetQueuedCompletionStatus(_hIocp, OUT & numOfBytes, OUT reinterpret_cast<PULONG_PTR>(&iocpObject), OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), time_limit))
	{
		iocpObject->CheckCompletionWork(iocpEvent, numOfBytes);
	}
	else
	{
		int32 errCode = ::WSAGetLastError();
		switch (errCode)
		{
		case WAIT_TIMEOUT:
			return false;
		default:
			// TODO : 로그 찍기
			iocpObject->CheckCompletionWork(iocpEvent, numOfBytes);
			break;
		}
	}

	return true;
}
