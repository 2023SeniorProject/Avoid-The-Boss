#pragma once
#include "IocpCore.h"
#include "Session.h"

enum class INTERACTION_TYPE : uint8 {
	ATTACK_EVENT,
	COOLTIME_EVENT,
	SWITCH_START_EVENT,
	SWITCH_AVAILABLE,
	SWITCH_UNAVAILABLE,
	SWITCH_END_EVENT, 
	SWITCH_ACTIVATE_EVENT,

};

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

class moveEvent : public queueEvent // 33 ms ���� �����Ѵ�.
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



class SwitchInteractionEvent : public queueEvent
{
public:
	SwitchInteractionEvent() {};
	virtual ~SwitchInteractionEvent() {};
	uint8 eventId;
	uint8 switchIdx;
public:
	virtual void Task() 
	{ 
		int16 roomNum = ServerIocpCore._clients[sid]->_myRm;
		switch ((INTERACTION_TYPE)eventId)
		{
		case INTERACTION_TYPE::SWITCH_START_EVENT:
		{
			if (!ServerIocpCore._rmgr->GetRoom(sid)._switchs[switchIdx]._IsActive &&
				!ServerIocpCore._rmgr->GetRoom(sid)._switchs[switchIdx]._IsOnInteraction) // ������ ��ȣ�ۿ��� ������ ���
			{

				// ���� �� ��ȣ�ۿ� ���·� ����
				if (!ServerIocpCore._rmgr->GetRoom(sid)._switchs[switchIdx].CanInteraction(sid))
				{
					std::cout << "Bug Detected\n";
					break;
				}
				else
				{
					ServerIocpCore._rmgr->GetRoom(sid)._switchs[switchIdx].SwitchInterationOn();
					SC_EVENTPACKET packet;
					packet.size = sizeof(SC_EVENTPACKET);
					packet.type = S_PACKET_TYPE::GAMEEVENT;
					packet.eventId = eventId; // 0: ������ ���� // ������ ���� // 1: ������ �Ϸ� // 2: ����� ���� ó�� // 3: ����� ���� ��Ÿ�� 
					ServerIocpCore._rmgr->GetRoom(sid).BroadCasting(&packet);
				}
			}
		}
		break;
		case INTERACTION_TYPE::SWITCH_END_EVENT: // ��ȣ�ۿ� ���߿� ���� ���
			if (!ServerIocpCore._rmgr->GetRoom(sid)._switchs[switchIdx]._IsActive &&
				ServerIocpCore._rmgr->GetRoom(sid)._switchs[switchIdx]._IsOnInteraction) // ������ ��ȣ�ۿ��� ���߿� �߻��� ���
			{
				// ��ȣ�ۿ� ���·� ����
				ServerIocpCore._rmgr->GetRoom(sid)._switchs[switchIdx].ResetGuage(); // ������ ������ �ʱ�ȭ
				SC_EVENTPACKET packet;
				packet.size = sizeof(SC_EVENTPACKET);
				packet.type = S_PACKET_TYPE::GAMEEVENT;
				packet.eventId = eventId; // 0: ������ ���� // ������ ���� // 1: ������ �Ϸ� // 2: ����� ���� ó�� // 3: ����� ���� ��Ÿ�� 
				ServerIocpCore._rmgr->GetRoom(sid).BroadCasting(&packet);
			}
			break;
		case INTERACTION_TYPE::SWITCH_ACTIVATE_EVENT:
		{
			
				SC_EVENTPACKET packet;
				packet.size = sizeof(SC_EVENTPACKET);
				packet.type = S_PACKET_TYPE::GAMEEVENT;
				packet.eventId = eventId; // 0: ������ ���� // ������ ���� // 1: ������ �Ϸ� // 2: ����� ���� ó�� // 3: ����� ���� ��Ÿ�� 
				ServerIocpCore._rmgr->GetRoom(sid).BroadCasting(&packet);
		}
		break;
		default:
			std::cout << "UnKnown Game Event Please Check Your Packet Type\n";
			break;
		}
	};
	
};

