#pragma once
#include "IocpCore.h"
#include "Session.h"

class ServerSession;

class ClientManager //:public IocpObject
{
public: // �������̽� ������ ����
	// ����ϰ� �ִ� iocObject�� �߻� �Լ����� �������̵�
	//virtual HANDLE GetHandle() override;
	//virtual void Processing(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;
public:
	void InitConnect(const char* address);
	void DoConnect(const char* idPw);
	void DoSend(void* packet);
public:
	SOCKADDR_IN _serveraddr;
	ServerSession _clientSession;
};
