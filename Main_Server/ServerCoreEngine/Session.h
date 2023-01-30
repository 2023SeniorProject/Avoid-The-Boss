#pragma once
#include "IocpCore.h"
class Session : public IocpObject
{
public:
	Session();
	~Session();
public:
	virtual HANDLE GetHandle() override;
	virtual void CheckCompletionWork(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;

	SOCKET GetSock() { return _sock; }
public:
	char _recvBuf[BUFSIZE];
	int32 _cid;
private:
	SOCKET _sock = INVALID_SOCKET;
};

