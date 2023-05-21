#pragma once
// 0 1 2
class CPlayer;

class queueEvent
{
public:
	int64 generateTime = 0.f;
	CPlayer* player = nullptr;
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
	XMFLOAT3 _pos {0,0,0};

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

