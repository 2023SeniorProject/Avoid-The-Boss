#pragma once
// 0 1 2
class CPlayer;

class queueEvent
{
public:
	int64 generateTime = 0.f;
	
public:
	queueEvent() {};
	virtual ~queueEvent() {};
	virtual void Task() {};
};

class moveEvent : public queueEvent // 33 ms ���� �����Ѵ�.
{
public:
	moveEvent() { };
	virtual ~moveEvent() {};
	CPlayer* player = nullptr;
	uint8 _key = 0;
	XMFLOAT3 _dir{ 0,0,0 };
public:
	virtual void Task();
};

class posEvent : public queueEvent // 33 ms ���� �����Ѵ�.
{
public:
	posEvent() { };
	virtual ~posEvent() {};
	XMFLOAT3 _pos {0,0,0};
	CPlayer* player = nullptr;
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

template<class T>
class DelayEvent : public queueEvent
{
	T _packet;

public:
	~DelayEvent() {};
	DelayEvent(T packet)
	{
		_packet = packet;
	}
public:
	virtual void Task()
	{
		clientCore.DoSend(&_packet);
	};
};

