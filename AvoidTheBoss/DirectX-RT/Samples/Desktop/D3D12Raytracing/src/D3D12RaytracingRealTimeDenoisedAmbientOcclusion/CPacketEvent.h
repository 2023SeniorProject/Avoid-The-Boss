#pragma once
#include "clientIocpCore.h"


class queueEvent
{
public:
	int64 generateTime = 0;

public:
	queueEvent() {};
	virtual ~queueEvent() {};
	virtual void Task() {};
};

class moveEvent : public queueEvent // 33 ms 마다 전송한다.
{
public:
	moveEvent() { };
	virtual ~moveEvent() {};
	//CPlayer* player = nullptr;
	uint8 _key = 0;
	XMFLOAT3 _dir{ 0,0,0 };
public:
	virtual void Task();
};

class posEvent : public queueEvent // 33 ms 마다 전송한다.
{
public:
	posEvent() { };
	virtual ~posEvent() {};
	XMFLOAT3 _pos{ 0,0,0 };
	//CPlayer* player = nullptr;
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

class FrameEvent : public queueEvent
{
public:
	FrameEvent() {};
	FrameEvent(int32 wf) : _wf(wf) {}
	virtual ~FrameEvent() {};
	int32 _wf = -1;
public:
	virtual void Task();
};


class DelayEvent : public queueEvent
{
	C2S_ATTACK _packet;

public:
	~DelayEvent() {};
	DelayEvent(C2S_ATTACK packet)
	{
		_packet = packet;
	}
public:
	virtual void Task()
	{

		clientCore.DoSend(&_packet);
		std::cout << "SendAttack";
	}
};
