#pragma once
#include "IocpCore.h"
#include "Session.h"
#include "CSIocpCore.h"
// 0 1 2


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
	uint8 eventId = -1;
public:
	virtual void Task() 
	{ 
		int16 roomNum = ServerIocpCore._clients[sid]->_myRm;
		switch ((INTERACTION_TYPE)eventId)
		{
		case INTERACTION_TYPE::SWITCH_ONE_START_EVENT:
		case INTERACTION_TYPE::SWITCH_TWO_START_EVENT:
		case INTERACTION_TYPE::SWITCH_THREE_START_EVENT:
		{
			if (!ServerIocpCore._rmgr->GetRoom(sid)._switchs[eventId - 2]._IsActive &&
				!ServerIocpCore._rmgr->GetRoom(sid)._switchs[eventId - 2]._IsOnInteraction) // ������ ��ȣ�ۿ��� ������ ���
			{

				// ���� �� ��ȣ�ۿ� ���·� ����
				if (!ServerIocpCore._rmgr->GetRoom(sid)._switchs[eventId -2].CanInteraction(sid))
				{
					std::cout << "Bug Detected\n";
					break;
				}
				else
				{
					std::cout << (int)(eventId - 2) << ") Switch Interaction On\n";
					ServerIocpCore._rmgr->GetRoom(sid)._switchs[eventId - 2].SwitchInteractionOn(true);
					SC_EVENTPACKET packet;
					packet.size = sizeof(SC_EVENTPACKET);
					packet.type = SC_PACKET_TYPE::GAMEEVENT;
					packet.eventId = eventId; // 0: ������ ���� // ������ ���� // 1: ������ �Ϸ� // 2: ����� ���� ó�� // 3: ����� ���� ��Ÿ�� 
					ServerIocpCore._rmgr->GetRoom(sid).BroadCastingExcept(&packet , sid);
				}
			}
		}
		break;
		case INTERACTION_TYPE::SWITCH_ONE_END_EVENT: // ��ȣ�ۿ� ���߿� ���� ���
		case INTERACTION_TYPE::SWITCH_TWO_END_EVENT: // ��ȣ�ۿ� ���߿� ���� ���
		case INTERACTION_TYPE::SWITCH_THREE_END_EVENT: // ��ȣ�ۿ� ���߿� ���� ���
			if (!ServerIocpCore._rmgr->GetRoom(sid)._switchs[eventId - 5]._IsActive &&
				ServerIocpCore._rmgr->GetRoom(sid)._switchs[eventId - 5]._IsOnInteraction) // ������ ��ȣ�ۿ��� ���߿� �߻��� ���
			{
				std::cout << "switch[" << eventId - 5 << "] " << "Cancel \n";
				// ��ȣ�ۿ� ���·� ����
				ServerIocpCore._rmgr->GetRoom(sid)._switchs[eventId - 5].SwitchInteractionOn(false);
				ServerIocpCore._rmgr->GetRoom(sid)._switchs[ eventId - 5].ResetGuage(); // ������ ������ �ʱ�ȭ
				SC_EVENTPACKET packet;
				packet.size = sizeof(SC_EVENTPACKET);
				packet.type = SC_PACKET_TYPE::GAMEEVENT;
				packet.eventId = eventId; // 0: ������ ���� // ������ ���� // 1: ������ �Ϸ� // 2: ����� ���� ó�� // 3: ����� ���� ��Ÿ�� 
				ServerIocpCore._rmgr->GetRoom(sid).BroadCastingExcept(&packet,sid);
			}
			break;
		default:
			std::cout << "UnKnown Game Event Please Check Your Packet Type\n";
			break;
		}
	};
	
};

