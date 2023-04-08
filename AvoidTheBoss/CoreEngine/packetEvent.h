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
	XMFLOAT3 predictPos;
public:
	virtual void Task()
	{
		std::lock_guard<std::mutex> plg(ServerIocpCore._clients[sid]->_playerLock);
		// to do move Player in gameLogic
		int16 roomNum = ServerIocpCore._clients[sid]->_myRm;

		S2C_POS packet;
		packet.size = sizeof(S2C_DIR);
		packet.sid = sid;
		packet.position = predictPos;
		ServerIocpCore._rmgr->GetRoom(roomNum).BroadCasting(&packet);
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