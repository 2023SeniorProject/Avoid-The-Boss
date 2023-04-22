#pragma once
#include "IocpCore.h"
#include "Session.h"

enum class INTERACTION_TYPE : uint8 {  ATTACK_EVENT, COOLTIME_EVENT, SWITCH_START_EVENT,SWITCH_END_EVENT, SWITCH_ACTIVATE_EVENT};

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
		
		if (_key == 0)
		{
			S2C_POS packet;
			packet.sid = sid;
			packet.size = sizeof(S2C_POS);
			packet.type = S_PACKET_TYPE::SPOS;
			packet.x = ServerIocpCore._rmgr->GetRoom(roomNum).GetMyPlayerFromRoom(sid).GetPosition().x;
			packet.z = ServerIocpCore._rmgr->GetRoom(roomNum).GetMyPlayerFromRoom(sid).GetPosition().z;
			std::cout << "[" << sid << "] (" << packet.x << "," << packet.z << ")\n";
			//ServerIocpCore._rmgr->GetRoom(roomNum).BroadCasting(&packet);
		}
	};
	
};



class InteractionEvent : public queueEvent
{
public:
	InteractionEvent() {};
	virtual ~InteractionEvent() {};
	uint8 eventId;
public:
	virtual void Task() 
	{ 
		switch ((INTERACTION_TYPE)eventId)
		{
		case INTERACTION_TYPE::ATTACK_EVENT:
			break;
		case INTERACTION_TYPE::SWITCH_START_EVENT:
			break;
		case INTERACTION_TYPE::SWITCH_END_EVENT:
			break;
		case INTERACTION_TYPE::SWITCH_ACTIVATE_EVENT:
			break;
		default:
			break;
		}
		int16 roomNum = ServerIocpCore._clients[sid]->_myRm;
		SC_EVENTPACKET packet;
		packet.size = sizeof(SC_EVENTPACKET);
		packet.type = S_PACKET_TYPE::GAMEEVENT;
		packet.eventId = eventId; // 0: 발전기 시작 // 발전기 종료 // 1: 발전기 완료 // 2: 사장님 공격 처리 // 3: 사장님 공격 쿨타임 
		ServerIocpCore._rmgr->GetRoom(sid).BroadCasting(&packet);
	};
	
};

