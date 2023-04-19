#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"
#include "Session.h"
#include "CollisionDetector.h"
#include "SocketUtil.h"

SIocpCore ServerIocpCore;

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
				std::cout << "Time Out Plz Check Your Network Condition" << std::endl;
				return false;
			default:
				// TODO : �α� ���
			{
				std::cout << ::WSAGetLastError() << "\n";
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
	BoxTree = new OcTree(XMFLOAT3(0, 0, 0), 60);
	BoxTree->BuildTree();
	BoxTree->ReadBoundingBoxInfoFromFile("bounding_boxes.txt");
}

SIocpCore::~SIocpCore()
{
	delete _rmgr;
}

void SIocpCore::Disconnect(int32 sid)
{
	std::cout << "[" << _clients[sid]->_cid << "] Disconnected" << std::endl;
	if(sid >= 0 && _clients[sid]->_myRm != -1) _rmgr->ExitRoom(sid, _clients[sid]->_myRm);
	{
		std::unique_lock<std::shared_mutex> disconnectLock(_lock);
	
		_cList.erase(_clients[sid]->_cid);
		_clients.erase(sid);
	}
}

