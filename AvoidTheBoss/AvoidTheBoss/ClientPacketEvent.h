#pragma once
// 0 1 2
#include "Player.h"
class queueEvent
{
public:
	int64 generateTime = 0.f;
	CPlayer* player;
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
	uint8 _key;
	XMFLOAT3 _dir{ 0,0,0 };
public:
	virtual void Task()
	{
		player->SetDirection(_dir);
		player->Move(_key, PLAYER_VELOCITY);
	};

};

class posEvent : public queueEvent // 33 ms 마다 전송한다.
{
public:
	posEvent() { };
	virtual ~posEvent() {};
	XMFLOAT3 _pos {0,0,0};

public:
	virtual void Task()
	{
		XMFLOAT3 curPos = player->GetPosition();
		XMFLOAT3 distance = Vector3::Subtract(curPos, _pos);
		if (Vector3::Length(distance) > 0.2f)
		{
			std::cout << "Mass Offset Detected. Reseting Pos\n";
			player->MakePosition(XMFLOAT3(_pos.x, _pos.y,_pos.z));
		}
	};
	
};
class SwitchInteractionEvent : public queueEvent
{
public:
	SwitchInteractionEvent() {};
	virtual ~SwitchInteractionEvent() {};
	uint8 eventId = -1;
public:
	virtual void Task() 
	{ 
		
		switch ((EVENT_TYPE)eventId)
		{
		case EVENT_TYPE::SWITCH_ONE_START_EVENT:
		case EVENT_TYPE::SWITCH_TWO_START_EVENT:
		case EVENT_TYPE::SWITCH_THREE_START_EVENT:
		{
			
		}
		break;
		case EVENT_TYPE::SWITCH_ONE_END_EVENT: // 상호작용 도중에 끝낸 경우
		case EVENT_TYPE::SWITCH_TWO_END_EVENT: // 상호작용 도중에 끝낸 경우
		case EVENT_TYPE::SWITCH_THREE_END_EVENT: // 상호작용 도중에 끝낸 경우
		
			break;
		default:
			std::cout << "UnKnown Game Event Please Check Your Packet Type\n";
			break;
		}
	};
	
};

