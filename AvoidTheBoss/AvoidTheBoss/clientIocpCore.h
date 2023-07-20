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
	void DoSend(void* packet) { _client->DoSend(packet); }
	void DoDelaySendTask() { _client->DoDelayTask(); }
	void DoDelaySend(C2S_ATTACK packet, float after) { _client->DoDelaySend(packet, after); }
	virtual void Disconnect(int32 sid) override;
private:
	CSession* _client;
	SOCKADDR_IN _serveraddr;

};

extern class CCIocpCore clientCore;