#pragma once
#include "Session.h"
#include "Player.h"
#include "DummyPlayer.h"

// =============== new Client Session ================
// ===================================================
// ===================================================

class CClientSession : public IocpObject
{
public:
	CClientSession();

	virtual ~CClientSession();
public:
	// 세션 인터페이스
	virtual HANDLE GetHandle() override;
	virtual void Processing(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;
public:
	// 세션 정보를 얻어 내거나 세팅할 수 있는 함수들
	SOCKET GetSock() { return _sock; }
	bool DoSend(void* packet);
	bool DoRecv();
	void ProcessPacket(char*);
	void PrintPInfo()
	{
		std::cout << _other->GetVelocity().x << _other->GetVelocity().z << std::endl;
	}
public:
	int32 _cid = -1;
	int32 _sid = -1;
	int16 _myRm = -1;
	int32 _prev_remain = 0;
	int8 _curScene = -1;
public:
	SOCKET _sock = INVALID_SOCKET;

	CCubePlayer* _player = nullptr;
	
	DummyCubePlayer* _other = nullptr;
	std::mutex _otherLock;

	RecvEvent _rev;
	RWLOCK;
};

// ========= new Iocp Core ==============

class CIocpCore : public IocpCore
{
public:
	CIocpCore();
	~CIocpCore();
	void InitConnect(const char* address);
	void DoConnect(void* loginInfo);
	virtual void Disconnect(int32 sid) override;
public:
	RWLOCK;
	CClientSession* _client;
	SOCKADDR_IN _serveraddr;

};

extern class CIocpCore clientIocpCore;