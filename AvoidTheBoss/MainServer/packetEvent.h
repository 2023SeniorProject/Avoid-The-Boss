#pragma once
#include "IocpCore.h"
#include "Session.h"
#include "CSIocpCore.h"
// 0 1 2


class queueEvent
{
public:
	int64 generateTime = 0.f;
	int32 _sid = -1;
public:
	queueEvent() {};
	virtual ~queueEvent() {};
	virtual void Task() {};
};

class moveEvent : public queueEvent // 33 ms 마다 전송한다.
{
public:
	moveEvent() { };
	moveEvent(int32 sid, uint8 key, XMFLOAT3 dir) : _key(key), _dir(dir) { _sid = sid; };
	virtual ~moveEvent() {};
	XMFLOAT3 _dir {0,0,0};
	uint8 _key = 0;
public:
	virtual void Task();
};



class InteractionEvent : public queueEvent
{
public:
	InteractionEvent() {};
	virtual ~InteractionEvent() {};
	uint8 eventId = -1;
public:
	virtual void Task();
};

