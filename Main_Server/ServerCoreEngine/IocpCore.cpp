#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"

IocpCore GIocpCore;

IocpCore::IocpCore()
{
	_hIocp = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0); // iocp �ڵ� ����
	ASSERT_CRASH(_hIocp != INVALID_HANDLE_VALUE); 
}

IocpCore::~IocpCore()
{
	::CloseHandle(_hIocp);
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
	//IocpObject* iocpObject = nullptr; // �ϰ��� �Ϸ�� iocpObject�� ������ �����ϱ� ���� IocpObject
	IocpEvent* iocpEvent = nullptr; // �ϰ��� �Ϸ�� iocpEvent�� ����(Accept�ΰ�?)
	ULONG_PTR key(0);
	BOOL retVal = ::GetQueuedCompletionStatus(_hIocp, OUT & numOfBytes, 
		/*key*/&key /*&iocpObject*/, // ������ �̷��� iocpObject�� ���ڷ� �Ѱ��ְ� �Ǹ�, �ٸ� �����忡�� �� ������Ʈ�� �������� ��, ������ ���� ���� �ִ�. --> 
		//���ʿ� iocpEvent���� �ش� iocp��ü�鿡 ���� ����(�ش� �̺�Ʈ�� ȣ���� ���� iocp��ü��)�� ��� �ֵ�������.
		OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), time_limit);
	if (retVal) // ���� �ߴ°�?
	{
		IocpObjRef iocpObject = iocpEvent->_ev_owner;// ���⼭ � iocpObject(����, ������)�� ������ ������ ���� ����
		iocpObject->Processing(iocpEvent, numOfBytes); // �����ϸ� �������� ���μ����� �����غ���
	}
	else // �����ߴٸ� �����ڵ� Ȯ��
	{
		int32 errCode = ::WSAGetLastError();
		switch (errCode)
		{
			case WAIT_TIMEOUT: // time_limit�� INFINITE�� �ƴ� ��� ==> ���߿� ���� ���� ��, ���� �ð��� ���� ���� ����
				return false;
			default:
				// TODO : �α� ���
			{
				IocpObjRef iocpObject = iocpEvent->_ev_owner;
				iocpObject->Processing(iocpEvent, numOfBytes);
				break;
			}
		}
	}

	return true;
}
