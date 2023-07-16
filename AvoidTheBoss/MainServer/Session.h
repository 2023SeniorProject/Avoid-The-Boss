#pragma once

#include "IocpEvent.h"
#include "ASession.h"

// 서버에서 클라이언트 소켓을 관리할 클래스
// 마찬가지로 Iocp에 등록할 대상이기 때문에 IocpObject에 해당된다


class ServerSession : public ASession
{
public:
	ServerSession();
	virtual ~ServerSession();
public:
	// 세션 인터페이스
	virtual HANDLE GetHandle() override;
	virtual void Processing(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;
public:
	// 세션 정보를 얻어 내거나 세팅할 수 있는 함수들
	SOCKET GetSock() { return _sock; }
	bool DoSend(void* packet);
	bool DoRecv();
	void DoSendLoginPacket(bool isSuccess);
	void ProcessPacket(char*);
public:
	int16 _myRm = -1;
	int32 _curPage = 0;
	int32 _prev_remain = 0;
public:
	
};


