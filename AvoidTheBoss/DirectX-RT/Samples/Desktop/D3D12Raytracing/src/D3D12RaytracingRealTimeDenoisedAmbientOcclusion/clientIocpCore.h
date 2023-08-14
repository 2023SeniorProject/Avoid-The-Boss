#pragma once
#include "stdafx.h"
#include "CSession.h"
#include "IocpCore.h"


class DXSample;
class CCIocpCore : public IocpCore
{
	friend class CSession;
public:
	CCIocpCore();
	~CCIocpCore();
	void InitConnect(const char* address);
	void DoConnect(void* loginInfo, DXSample* sample);
	void DoSend(void* packet) { _client->DoSend(packet); }
	virtual bool Processing(uint32_t timelimit = INFINITE);
	virtual void Disconnect(int32 sid) override;
private:
	CSession* _client;
	SOCKADDR_IN _serveraddr;

};

extern class CCIocpCore clientCore;
