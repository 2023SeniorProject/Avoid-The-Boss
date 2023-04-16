#pragma once
#include "GameFramework.h"
#include "Session.h"
#include "Player.h"
// =============== new Client Session ================
// ===================================================
// ===================================================

extern HWND g_hwnd;

class CSession : public IocpObject
{
	
public:
	CSession();
	virtual ~CSession();
public:
	// ���� �������̽�
	virtual HANDLE GetHandle() override;
	virtual void Processing(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;
public:
	// ���� ������ ��� ���ų� ������ �� �ִ� �Լ���
	SOCKET GetSock() { return _sock; }
	bool DoSend(void* packet);
	bool DoRecv();
	void ProcessPacket(char*);
public:
	int32 _cid = -1;
	int32 _sid = -1;
	int16 _myRm = -1;
	int32 _prev_remain = 0;
	int16  _loginOk = -3;
public:
	SOCKET _sock = INVALID_SOCKET;
	RecvEvent _rev;
	RWLOCK;
};

// ========= new Iocp Core ==============

class ClientMananger : public IocpCore
{
	friend class CSession;
public:
	ClientMananger();
	~ClientMananger();
	void InitConnect(const char* address);
	void DoConnect(void* loginInfo);
	virtual void Disconnect(int32 sid) override;
public:
	CSession* _client;
	SOCKADDR_IN _serveraddr;

};

extern class ClientMananger clientCore;