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
			ServerIocpCore._rmgr->GetRoom(roomNum).BroadCastingExcept(&packet, sid);
		}
	};
	
};



class InteractionEvent : public queueEvent
{
public:
	InteractionEvent() {};
	virtual ~InteractionEvent() {};
	uint8 eventId = -1;
public:
	virtual void Task() 
	{ 
		int16 roomNum = ServerIocpCore._clients[sid]->_myRm;
		switch ((EVENT_TYPE)eventId)
		{
		//============= 스위치 관련 이벤트 ===================
		case EVENT_TYPE::SWITCH_ONE_START_EVENT:
		case EVENT_TYPE::SWITCH_TWO_START_EVENT:
		case EVENT_TYPE::SWITCH_THREE_START_EVENT:
		{
			if (!ServerIocpCore._rmgr->GetRoom(sid)._switchs[eventId - 2]._IsActive &&
				!ServerIocpCore._rmgr->GetRoom(sid)._switchs[eventId - 2]._IsOnInteraction) // 발전기 상호작용이 가능할 경우
			{

				// 검증 후 상호작용 상태로 변경
				if (!ServerIocpCore._rmgr->GetRoom(sid)._switchs[eventId -2].CanInteraction(sid))
				{
					std::cout << "Bug Detected\n";
					break;
				}
				else
				{
					ServerIocpCore._rmgr->GetRoom(sid)._switchs[eventId - 2].SwitchInteractionOn(true);
					SC_EVENTPACKET packet;
					packet.size = sizeof(SC_EVENTPACKET);
					packet.type = SC_PACKET_TYPE::GAMEEVENT;
					packet.eventId = eventId; // 0: 발전기 시작 // 발전기 종료 // 1: 발전기 완료 // 2: 사장님 공격 처리 // 3: 사장님 공격 쿨타임 
					ServerIocpCore._rmgr->GetRoom(sid).BroadCastingExcept(&packet , sid);
				}
			}
		}
		break;
		case EVENT_TYPE::SWITCH_ONE_END_EVENT: // 상호작용 도중에 끝낸 경우
		case EVENT_TYPE::SWITCH_TWO_END_EVENT: // 상호작용 도중에 끝낸 경우
		case EVENT_TYPE::SWITCH_THREE_END_EVENT: // 상호작용 도중에 끝낸 경우
			if (!ServerIocpCore._rmgr->GetRoom(sid)._switchs[eventId - 5]._IsActive &&
				ServerIocpCore._rmgr->GetRoom(sid)._switchs[eventId - 5]._IsOnInteraction) // 발전기 상호작용이 도중에 발생한 경우
			{
				std::cout << "switch[" << eventId - 5 << "] " << "Cancel \n";
				// 상호작용 상태로 변경
				ServerIocpCore._rmgr->GetRoom(sid)._switchs[eventId - 5].SwitchInteractionOn(false);
				ServerIocpCore._rmgr->GetRoom(sid)._switchs[ eventId - 5].ResetGuage(); // 발전기 게이지 초기화
				SC_EVENTPACKET packet;
				packet.size = sizeof(SC_EVENTPACKET);
				packet.type = SC_PACKET_TYPE::GAMEEVENT;
				packet.eventId = eventId; // 0: 발전기 시작 // 발전기 종료 // 1: 발전기 완료 // 2: 사장님 공격 처리 // 3: 사장님 공격 쿨타임 
				ServerIocpCore._rmgr->GetRoom(sid).BroadCastingExcept(&packet,sid);
			}
			break;
		// ============== 공격 관련 이벤트 ================
		case EVENT_TYPE::ATTACK_EVENT:
		{
			// 공격 이벤트 검증
			int32 myIdx = ServerIocpCore._rmgr->GetRoom(sid).GetMyPlayerFromRoom(sid).m_idx;
			XMFLOAT3 BossPos = ServerIocpCore._rmgr->GetRoom(sid).GetMyPlayerFromRoom(sid).GetPosition();
			XMFLOAT3 BossDir = ServerIocpCore._rmgr->GetRoom(sid).GetMyPlayerFromRoom(sid).m_xmf3Look;
			float rayDist = 5.0f;
			for (int i = 0; i < PLAYERNUM; ++i)
			{
				if (i == myIdx) continue;
				if (ServerIocpCore._rmgr->GetRoom(sid)._players[i].m_playerBV.Intersects(XMLoadFloat3(&BossPos), XMLoadFloat3(&BossDir), rayDist))
				{
					std::cout << ServerIocpCore._rmgr->GetRoom(sid)._players[i].m_idx << "Get Attacked\n";
					ServerIocpCore._rmgr->GetRoom(sid)._players[i].m_hp -= 1;
				}

			}
		}
		default:
			std::cout << "UnKnown Game Event Please Check Your Packet Type\n";
			break;
		}
	};
	
};

