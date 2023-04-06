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

class moveEvent : public queueEvent
{
public:
	moveEvent() { };
	virtual ~moveEvent() {};
	uint8 key = 0;
public:
	virtual void Task()
	{
		std::lock_guard<std::mutex> plg(ServerIocpCore._clients[sid]->_playerLock);
		// to do move Player in gameLogic
		int16 roomNum = ServerIocpCore._clients[sid]->_myRm;
		ServerIocpCore._rmgr->GetRoom(roomNum).GetMyPlayerFromRoom(sid).Move(key, PLAYER_VELOCITY);
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
		int16 roomNum = ServerIocpCore._clients[sid]->_myRm;
		ServerIocpCore._rmgr->GetRoom(roomNum).GetMyPlayerFromRoom(sid).Rotate(0,angleY,0);
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