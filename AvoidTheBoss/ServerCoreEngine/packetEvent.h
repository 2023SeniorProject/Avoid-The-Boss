#pragma once
#include "IocpCore.h"
#include "Session.h"
enum EVENT_TYPE : int8 { MOVE_EVENT, ATTACK_EVENT};
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
	int8 type = MOVE_EVENT;
	int32 sid;
	XMFLOAT3 velocity;
public:
	virtual void Task()
	{
		std::lock_guard<std::mutex> plg(ServerIocpCore._clients[sid]->_playerLock);
		ServerIocpCore._clients[sid]->_playerInfo.SetVelocity(velocity);
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