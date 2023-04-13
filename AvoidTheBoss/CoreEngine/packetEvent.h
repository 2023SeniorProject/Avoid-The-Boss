#pragma once
#include "IocpCore.h"
#include "Session.h"

enum EVENT_TYPE : int8 { MOVE_EVENT, ATTACK_EVENT, ROTATE_EVENT};

// queue
class queueEvent
{
public:
	int64 generateTime = 0.f;
	int32 sid = -1;
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
	XMFLOAT3 _dir {0,0,0};
	uint8 _key = 0;
public:
	virtual void Task()
	{
		// to do move Player in gameLogic
		int16 roomNum = ServerIocpCore._clients[sid]->_myRm;
		ServerIocpCore._rmgr->GetRoom(roomNum).GetMyPlayerFromRoom(sid).SetDirection(_dir);
		ServerIocpCore._rmgr->GetRoom(roomNum).GetMyPlayerFromRoom(sid).Move(_key, PLAYER_VELOCITY);

	};
	
};

class rotateEvent : public queueEvent
{
public:
	rotateEvent() { };
	virtual ~rotateEvent() {};
	float angleY = 0.f;
public:
	virtual void Task()
	{
	};

};

class attackEvent : public queueEvent
{
public:
	attackEvent() {};
	virtual ~attackEvent() {};
	int8 type = ATTACK_EVENT;
public:
	virtual void Task() { };
};