#pragma once
#include "IocpCore.h"
#include "Session.h"

class GameSession;

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
	static void SetScene(int32);
public:
	SOCKADDR_IN _serveraddr;
	GameSession _clientSession;
	static int32 _scene;
	
};

