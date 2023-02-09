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
	virtual void Disconnect(int32 sid);
public:
	RWLOCK;
	unordered_map<int32, ServerSession*> _clients;
	std::set<uint16> _cList;
	RoomManager* _rmgr;
};

class ClientSession;

class CIocpCore : public IocpCore
{
public:
	CIocpCore();
	~CIocpCore();
	void InitConnect(const char* address);
	void DoConnect(void* loginInfo);
	virtual void Disconnect(int32 sid);
public:
	RWLOCK;
	ClientSession* _client;
	SOCKADDR_IN _serveraddr;
};



extern SIocpCore ServerIocpCore;

extern CIocpCore ClientIocpCore;
