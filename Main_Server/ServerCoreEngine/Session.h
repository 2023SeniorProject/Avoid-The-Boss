#pragma once
#include "IocpCore.h"
// 서버에서 클라이언트 소켓을 관리할 클래스
// 마찬가지로 Iocp에 등록할 대상이기 때문에 IocpObject에 해당된다
class Session : public IocpObject
{
public:
	Session();
	virtual ~Session();
public:
	// 세션 인터페이스
	virtual HANDLE GetHandle() override;
	virtual void Processing(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;
public:
	// 세션 정보를 얻어 내거나 세팅할 수 있는 함수들
	SOCKET GetSock() { return _sock; }
public:
	char _recvBuf[BUFSIZE];
	int32 _cid;
private:
	SOCKET _sock = INVALID_SOCKET;
};

