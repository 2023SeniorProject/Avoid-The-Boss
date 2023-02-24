#pragma once
#include "IocpCore.h"
#include "Session.h"
enum EVENT_TYPE : int8 { MOVE_EVENT, ATTACK_EVENT, ROTATE_EVENT};
// queue
class queueEvent
{
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
	int32 sid = -1;
	uint8 key = 0;
public:
	virtual void Task()
	{
		std::lock_guard<std::mutex> plg(ServerIocpCore._clients[sid]->_playerLock);
		ServerIocpCore._clients[sid]->_playerInfo.Move(key, UNIT * 1.2f);
	};
	
};

class rotateEvent : public queueEvent
{
public:
	rotateEvent() { };
	virtual ~rotateEvent() {};
	int32 sid = -1;
	float angleY = 0.f;
public:
	virtual void Task()
	{
		std::lock_guard<std::mutex> plg(ServerIocpCore._clients[sid]->_playerLock);
		ServerIocpCore._clients[sid]->_playerInfo.Rotate(0, angleY ,0);
	
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