#pragma once
#include "IocpObject.h"

class ASession : public IocpObject
{
public:
	int32 _cid = -1;
	int32 _sid = -1;
public:
	SOCKET _sock = INVALID_SOCKET;
	RecvEvent _rev;
	RWLOCK;
public:
	virtual HANDLE GetHandle() { return 0; };
	virtual void Processing(class IocpEvent* iocpEvent, int32 numBytes) {} // 어떤 일감으로  Iocp에 등록했니?
};

