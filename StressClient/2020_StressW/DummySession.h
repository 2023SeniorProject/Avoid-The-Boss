#pragma once
#include "ASession.h"



class DummySession : public ASession
{
	int32  _sid;
	SOCKET _socket;
public:
	void SendPacket(void* packet);
	void RecvPacket();
	virtual void Processing(class IocpEvent* iocpEvent, int32 numBytes) override;
	void ProcessPacket();
};

