#pragma once
#include "GameFramework.h"
#include "ClientSession.h"
#include "IocpCore.h"
#include "Player.h"
// =============== new Client Session ================
// ===================================================
// ===================================================

extern HWND g_hwnd;

// ========= new Iocp Core ==============

class CCIocpCore : public IocpCore
{
	friend class CSession;
public:
	CCIocpCore();
	~CCIocpCore();
	void InitConnect(const char* address);
	void DoConnect(void* loginInfo);
	virtual void Disconnect(int32 sid) override;
public:
	CSession* _client;
	SOCKADDR_IN _serveraddr;

};

extern class CCIocpCore clientCore;