#include "pch.h"
#include "packetEvent.h"

void InteractionEvent::Task()
{
	int16 roomNum = ServerIocpCore._clients[_sid]->_myRm;
	Room& targetRoom = ServerIocpCore._rmgr->GetRoom(roomNum);
	switch ((EVENT_TYPE)eventId)
	{
		//============= ����ġ ���� �̺�Ʈ ===================
	case EVENT_TYPE::SWITCH_ONE_START_EVENT:
	case EVENT_TYPE::SWITCH_TWO_START_EVENT:
	case EVENT_TYPE::SWITCH_THREE_START_EVENT:
	{
		SGenerator& targetGen = targetRoom._generator[eventId - (uint8)EVENT_TYPE::SWITCH_ONE_START_EVENT];
		if (!targetGen._IsActive && !targetGen._IsOnInteraction) // ������ ��ȣ�ۿ��� ������ ���
		{
			// ���� �� ��ȣ�ۿ� ���·� ����
			if (!targetGen.CanInteraction(roomNum, _sid))
			{
				std::cout << "Bug Detected\n";
				break;
			}
			else
			{
				std::cout << "Gen\n";
				targetGen.GenInteractionOn(true);
				SC_EVENTPACKET packet;
				packet.size = sizeof(SC_EVENTPACKET);
				packet.type = (uint8)SC_PACKET_TYPE::GAMEEVENT;
				packet.eventId = eventId;

				S2C_ANIMPACKET animPacket;
				animPacket.size = sizeof(S2C_ANIMPACKET);
				animPacket.type = (uint8)S_PACKET_TYPE::ANIM;
				animPacket.idx = targetRoom.GetMyPlayerFromRoom(_sid).m_idx;
				animPacket.track = (uint8)ANIMTRACK::GEN_ANIM;

				targetRoom.BroadCastingExcept(&animPacket, _sid);
				targetRoom.BroadCastingExcept(&packet, _sid);
			}
		}
	}
	break;
	case EVENT_TYPE::SWITCH_ONE_END_EVENT: // ��ȣ�ۿ� ���߿� ���� ���
	case EVENT_TYPE::SWITCH_TWO_END_EVENT: // ��ȣ�ۿ� ���߿� ���� ���
	case EVENT_TYPE::SWITCH_THREE_END_EVENT: // ��ȣ�ۿ� ���߿� ���� ���
	{
		SGenerator& targetGen = targetRoom._generator[eventId - (uint8)EVENT_TYPE::SWITCH_ONE_END_EVENT];
		if (!targetGen._IsActive && targetGen._IsOnInteraction) // ������ ��ȣ�ۿ��� ���߿� �߻��� ���
		{
	
			// ��ȣ�ۿ� ���·� ����
			targetGen.GenInteractionOn(false);
			SC_EVENTPACKET packet;
			packet.size = sizeof(SC_EVENTPACKET);
			packet.type = (uint8)SC_PACKET_TYPE::GAMEEVENT;
			packet.eventId = eventId; // 0: ������ ���� // ������ ���� // 1: ������ �Ϸ� // 2: ����� ���� ó�� // 3: ����� ���� ��Ÿ��

			S2C_ANIMPACKET animPacket;
			animPacket.size = sizeof(S2C_ANIMPACKET);
			animPacket.type = (uint8)S_PACKET_TYPE::ANIM;
			animPacket.idx = _sid;
			animPacket.track = (uint8)ANIMTRACK::GEN_ANIM_CANCEL;

			targetRoom.BroadCastingExcept(&animPacket, _sid);
			targetRoom.BroadCastingExcept(&packet, _sid);
		}
	}
	break;

	case EVENT_TYPE::SWITCH_ONE_ACTIVATE_EVENT: // ��ȣ�ۿ� ���߿� ���� ���
	case EVENT_TYPE::SWITCH_TWO_ACTIVATE_EVENT: // ��ȣ�ۿ� ���߿� ���� ���
	case EVENT_TYPE::SWITCH_THREE_ACTIVATE_EVENT: // ��ȣ�ۿ� ���߿� ���� ���
	{
		std::cout << "Active\n";
		SGenerator& targetGen = targetRoom._generator[eventId - (uint8)EVENT_TYPE::SWITCH_ONE_ACTIVATE_EVENT];
		targetGen.GenActivate(true);
		SC_EVENTPACKET packet;
		packet.size = sizeof(SC_EVENTPACKET);
		packet.type = (uint8)SC_PACKET_TYPE::GAMEEVENT;
		packet.eventId = eventId;
		targetRoom.BroadCasting(&packet);
	}
	break;
	// ============== ���� ���� �̺�Ʈ ================
	case EVENT_TYPE::ATTACK_EVENT:
	{
		// ���� �̺�Ʈ ����
		int32 myIdx = targetRoom.GetMyPlayerFromRoom(_sid).m_idx;
		SPlayer& bossPlayer = targetRoom.GetMyPlayerFromRoom(_sid);
		XMFLOAT3 BossPos = bossPlayer.GetPosition();
		XMFLOAT3 BossDir = bossPlayer.m_xmf3Look;

		//����� �ִϸ��̼� ����� ���� �̺�Ʈ ��Ŷ
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
	// ============== ����� ���� �̺�Ʈ =============
	case EVENT_TYPE::RESCUE_PLAYER_ONE:
	case EVENT_TYPE::RESCUE_PLAYER_TWO:
	case EVENT_TYPE::RESCUE_PLAYER_THREE:
	case EVENT_TYPE::RESCUE_PLAYER_FOUR:
	{
		SPlayer& p = targetRoom._players[(int8)eventId - (int8)EVENT_TYPE::RESCUE_PLAYER_ONE];
		if (!p.m_bIsAwaking)
		{
			p.m_bIsAwaking = true;
			std::cout << "RESCUING\n";
		}

		SC_EVENTPACKET packet;
		packet.type = (uint8)SC_PACKET_TYPE::GAMEEVENT;
		packet.size = sizeof(SC_EVENTPACKET);
		packet.eventId = eventId;
		targetRoom.BroadCastingExcept(&packet, _sid);
	}
	break;
	case EVENT_TYPE::RESCUE_CANCEL_PLAYER_ONE:
	case EVENT_TYPE::RESCUE_CANCEL_PLAYER_TWO:
	case EVENT_TYPE::RESCUE_CANCEL_PLAYER_THREE:
	case EVENT_TYPE::RESCUE_CANCEL_PLAYER_FOUR:
	{
		SPlayer& p = targetRoom._players[(int8)eventId - (int8)EVENT_TYPE::RESCUE_CANCEL_PLAYER_ONE];
		if (p.m_bIsAwaking) p.m_bIsAwaking = false;
		std::cout << "RESCUE CANCEL\n";

		SC_EVENTPACKET packet;
		packet.type = (uint8)SC_PACKET_TYPE::GAMEEVENT;
		packet.size = sizeof(SC_EVENTPACKET);
		packet.eventId = eventId;
		targetRoom.BroadCastingExcept(&packet,_sid);

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
	Room& targetRoom = ServerIocpCore._rmgr->GetRoom(roomNum);
	SPlayer& targetPlayer = targetRoom.GetMyPlayerFromRoom(_sid);
	
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

