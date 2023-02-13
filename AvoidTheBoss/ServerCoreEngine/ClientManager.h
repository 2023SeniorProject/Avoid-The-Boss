#pragma once
#include "IocpCore.h"
#include "Session.h"

class ServerSession;

class ClientManager //:public IocpObject
{
public: // 인터페이스 구현할 예정
	// 상속하고 있는 iocObject의 추상 함수들을 오버라이딩
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

