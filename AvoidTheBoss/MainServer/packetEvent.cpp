#include "pch.h"
#include "packetEvent.h"

void InteractionEvent::Task()
{
	int16 roomNum = ServerIocpCore._clients[_sid]->_myRm;
	Room& targetRoom = targetRoom;
	switch ((EVENT_TYPE)eventId)
	{
		//============= 스위치 관련 이벤트 ===================
	case EVENT_TYPE::SWITCH_ONE_START_EVENT:
	case EVENT_TYPE::SWITCH_TWO_START_EVENT:
	case EVENT_TYPE::SWITCH_THREE_START_EVENT:
	{
		if (!targetRoom._switchs[eventId - (uint8)EVENT_TYPE::SWITCH_ONE_START_EVENT]._IsActive &&
			!targetRoom._switchs[eventId - (uint8)EVENT_TYPE::SWITCH_ONE_START_EVENT]._IsOnInteraction) // 발전기 상호작용이 가능할 경우
		{

			// 검증 후 상호작용 상태로 변경
			if (!targetRoom._switchs[eventId - (uint8)EVENT_TYPE::SWITCH_ONE_START_EVENT].CanInteraction(roomNum, _sid))
			{
				std::cout << "Bug Detected\n";
				break;
			}
			else
			{
				targetRoom._switchs[eventId - (uint8)EVENT_TYPE::SWITCH_ONE_START_EVENT].SwitchInteractionOn(true);
				SC_EVENTPACKET packet;
				packet.size = sizeof(SC_EVENTPACKET);
				packet.type = (uint8)SC_PACKET_TYPE::GAMEEVENT;
				packet.eventId = eventId;

				S2C_SWITCH_ANIM animPacket;
				animPacket.size = sizeof(S2C_SWITCH_ANIM);
				animPacket.type = (uint8)S_PACKET_TYPE::SWITCH_ANIM;
				animPacket.idx = targetRoom.GetMyPlayerFromRoom(_sid).m_idx;

				targetRoom.BroadCastingExcept(&animPacket, _sid);
				targetRoom.BroadCastingExcept(&packet, _sid);
			}
		}
	}
	break;
	case EVENT_TYPE::SWITCH_ONE_END_EVENT: // 상호작용 도중에 끝낸 경우
	case EVENT_TYPE::SWITCH_TWO_END_EVENT: // 상호작용 도중에 끝낸 경우
	case EVENT_TYPE::SWITCH_THREE_END_EVENT: // 상호작용 도중에 끝낸 경우
	{
		if (!targetRoom._switchs[eventId - (uint8)EVENT_TYPE::SWITCH_ONE_END_EVENT]._IsActive &&
			targetRoom._switchs[eventId -  (uint8)EVENT_TYPE::SWITCH_ONE_END_EVENT]._IsOnInteraction) // 발전기 상호작용이 도중에 발생한 경우
		{
			std::cout << "switch[" << eventId - (uint8)EVENT_TYPE::SWITCH_ONE_END_EVENT << "] " << "Cancel \n";
			// 상호작용 상태로 변경
			targetRoom._switchs[eventId - 5].SwitchInteractionOn(false);
			targetRoom._switchs[eventId - 5].ResetGuage(); // 발전기 게이지 초기화
			SC_EVENTPACKET packet;
			packet.size = sizeof(SC_EVENTPACKET);
			packet.type = (uint8)SC_PACKET_TYPE::GAMEEVENT;
			packet.eventId = eventId; // 0: 발전기 시작 // 발전기 종료 // 1: 발전기 완료 // 2: 사장님 공격 처리 // 3: 사장님 공격 쿨타임

			S2C_SWITCH_ANIM packet2;
			packet2.size = sizeof(S2C_SWITCH_ANIM);
			packet2.type = (uint8)S_PACKET_TYPE::SWITCH_ANIM_CANCEL;
			packet2.idx = targetRoom.GetMyPlayerFromRoom(_sid).m_idx;
			targetRoom.BroadCastingExcept(&packet2, _sid);
			targetRoom.BroadCastingExcept(&packet, _sid);
		}
	}
	break;
	// ============== 공격 관련 이벤트 ================
	case EVENT_TYPE::ATTACK_EVENT:
	{
		// 공격 이벤트 검증
		int32 myIdx = targetRoom.GetMyPlayerFromRoom(_sid).m_idx;
		PlayerInfo& bossPlayer = targetRoom.GetMyPlayerFromRoom(_sid);
		XMFLOAT3 BossPos = bossPlayer.GetPosition();
		XMFLOAT3 BossDir = bossPlayer.m_xmf3Look;

		//사장님 애니메이션 재생을 위한 이벤트 패킷
		SC_EVENTPACKET packet;
		packet.type = (uint8)SC_PACKET_TYPE::GAMEEVENT;
		packet.size = sizeof(SC_EVENTPACKET);
		packet.eventId = (uint8)EVENT_TYPE::ATTACK_EVENT;
		targetRoom.BroadCastingExcept(&packet, _sid);


		float rayDist = 10.0f;
		for (int i = 0; i < PLAYERNUM; ++i)
		{
			if (i == myIdx) continue;
			if (targetRoom._players[i].m_playerBV.Intersects(XMLoadFloat3(&BossPos), XMLoadFloat3(&BossDir), rayDist))
			{
				std::cout << targetRoom._players[i].m_idx << "Get Attacked\n";
				targetRoom._players[i].m_hp -= 1;

				SC_EVENTPACKET packet;
				packet.type = (uint8)SC_PACKET_TYPE::GAMEEVENT;
				packet.size = sizeof(SC_EVENTPACKET);
				packet.eventId = (uint8)EVENT_TYPE::ATTACKED_PLAYER_ONE + i;
				targetRoom.BroadCasting(&packet);

				if (targetRoom._players[i].m_hp == 0)
				{
					targetRoom._players[i].m_behavior = (int32)PLAYER_BEHAVIOR::CRAWL;
				}
			}

		}
	}
	break;
	// ============== 깨우기 관련 이벤트 =============
	case EVENT_TYPE::RESCUE_PLAYER_ONE:
	case EVENT_TYPE::RESCUE_PLAYER_TWO:
	case EVENT_TYPE::RESCUE_PLAYER_THREE:
	case EVENT_TYPE::RESCUE_PLAYER_FOUR:
	{
		if (!targetRoom._players[(int8)eventId - (int8)EVENT_TYPE::RESCUE_PLAYER_ONE].m_bIsAwaking)
		{
			targetRoom._players[(int8)eventId - (int8)EVENT_TYPE::RESCUE_PLAYER_ONE].m_bIsAwaking = true;
			std::cout << "RESCUING\n";
		}
	}
	break;
	case EVENT_TYPE::RESCUE_CANCEL_PLAYER_ONE:
	case EVENT_TYPE::RESCUE_CANCEL_PLAYER_TWO:
	case EVENT_TYPE::RESCUE_CANCEL_PLAYER_THREE:
	case EVENT_TYPE::RESCUE_CANCEL_PLAYER_FOUR:
	{
		if (targetRoom._players[(int8)eventId - (int8)EVENT_TYPE::RESCUE_CANCEL_PLAYER_ONE].m_bIsAwaking)
		{
			targetRoom._players[(int8)eventId - (int8)EVENT_TYPE::RESCUE_CANCEL_PLAYER_ONE].m_bIsAwaking = true;
			targetRoom._players[(int8)eventId - (int8)EVENT_TYPE::RESCUE_CANCEL_PLAYER_ONE].rescueTime = 5.0;
		}
		std::cout << "RESCUE CANCEL\n";
	}
	break;
	default:
		std::cout << "UnKnown Game Event Please Check Your Packet Type\n";
		break;
	}
};

void moveEvent::Task()
{
	// to do move Player in gameLogic
	int16 roomNum = ServerIocpCore._clients[_sid]->_myRm;
	PlayerInfo& targetPlayer = ServerIocpCore._rmgr->GetRoom(roomNum).GetMyPlayerFromRoom(_sid);
	Room& targetRoom = targetRoom;
	targetPlayer.SetDirection(_dir);
	
	if (targetPlayer.m_idx == 0) targetPlayer.Move(_key, BOSS_VELOCITY);
	else targetPlayer.Move(_key, EMPLOYEE_VELOCITY);

	if (_key == 0)
	{
		S2C_POS packet;
		packet.sid = _sid;
		packet.size = sizeof(S2C_POS);
		packet.type = (uint8)S_PACKET_TYPE::SPOS;
		packet.x = targetPlayer.GetPosition().x;
		packet.z = targetPlayer.GetPosition().z;
		std::cout << "[" << _sid << "] (" << packet.x << "," << packet.z << ")\n";
		targetRoom.BroadCastingExcept(&packet, _sid);
	}
};

