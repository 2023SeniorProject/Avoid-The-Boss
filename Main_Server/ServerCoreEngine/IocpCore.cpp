#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"
#include "Session.h"

IocpCore GIocpCore;

IocpCore ClientIocpCore;


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
				return false;
			default:
				// TODO : �α� ���
			{
				std::cout << ::WSAGetLastError() << std::endl;
				
				//iocpObject->Processing(iocpEvent, numOfBytes);
				break;
			}
		}
	}
	
	if (numOfBytes == 0 && (iocpEvent->_comp == EventType::Recv || iocpEvent->_comp == EventType::Send))
	{
		//Disconnect
		//disconnect(static_cast<int>(key));
		GameSession* s = static_cast<GameSession*>(iocpObject);
		Disconnect(s->_sid);
		if (iocpEvent->_comp == EventType::Send) delete iocpEvent;
		return false;
	}

	iocpObject->Processing(iocpEvent, numOfBytes); // �����ϸ� �������� ���μ����� �����غ���
	
	return true;
}

void IocpCore::Disconnect(int32 sid)
{
	WRITE_LOCK;
	cout << "[" << _clients[sid]->_cid << "] Disconnected" << endl;
	delete _clients[sid];
	_clients[sid] = nullptr;
	//_clients.erase(sid);
}
