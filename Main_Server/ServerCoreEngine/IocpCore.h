#pragma once
// Worker Thread 기능을 클래스로 랩핑
#include "RoomManager.h"
// IOCP에 등록할 수 있는 모든 오브젝트에 관해서 정의

class IocpObject 
{
public:
	virtual HANDLE GetHandle() abstract;
	virtual void Processing(class IocpEvent* iocpEvent, int32 numBytes) abstract; // 어떤 일감으로  Iocp에 등록했니?
	// iocpEvent에 따라 적절한 일을 처리
};

class IocpCore
{

public:
	IocpCore();
	virtual ~IocpCore();
	HANDLE GetHandle() { return _hIocp; };
	bool Register(class IocpObject* iocpObj); // socket과 session을 등록하는 함수
	bool Processing(uint32_t limit_time = INFINITE); // 실질적으로 일하는 worker_thread들이 iocp에서 완료된 업무를 탐색하는 곳
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
