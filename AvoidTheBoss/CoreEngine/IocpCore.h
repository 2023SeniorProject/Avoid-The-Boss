#pragma once
// Worker Thread ����� Ŭ������ ����
#include "RoomManager.h"

// IOCP�� ����� �� �ִ� ��� ������Ʈ�� ���ؼ� ����
class IocpObject 
{
public:
	virtual HANDLE GetHandle() abstract;
	virtual void Processing(class IocpEvent* iocpEvent, int32 numBytes) abstract; // � �ϰ�����  Iocp�� ����ߴ�?
	// iocpEvent�� ���� ������ ���� ó��
};

class IocpCore
{

public:
	IocpCore();
	virtual ~IocpCore();
	HANDLE GetHandle() { return _hIocp; };
	bool Register(class IocpObject* iocpObj); // socket�� session�� ����ϴ� �Լ�
	bool Processing(uint32_t limit_time = INFINITE); // ���������� ���ϴ� worker_thread���� iocp���� �Ϸ�� ������ Ž���ϴ� ��
	virtual void Disconnect(int32 sid);
protected:
	HANDLE _hIocp = INVALID_HANDLE_VALUE;
};

class SIocpCore : public IocpCore
{

public:
	SIocpCore();
	~SIocpCore();
	virtual void Disconnect(int32 sid) override;
public:
	std::shared_mutex _lock;
	std::unordered_map<int32, ServerSession*> _clients;
	std::set<int32> _cList;
	RoomManager* _rmgr;
};


extern SIocpCore ServerIocpCore;

