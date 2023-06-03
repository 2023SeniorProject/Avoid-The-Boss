#include "pch.h"
#include "packetEvent.h"

void InteractionEvent::Task()
{
	int16 roomNum = ServerIocpCore._clients[_sid]->_myRm;
	Room& targetRoom = targetRoom;
	switch ((EVENT_TYPE)eventId)
	{
		//============= ����ġ ���� �̺�Ʈ ===================
	case EVENT_TYPE::SWITCH_ONE_START_EVENT:
	case EVENT_TYPE::SWITCH_TWO_START_EVENT:
	case EVENT_TYPE::SWITCH_THREE_START_EVENT:
	{
		if (!targetRoom._switchs[eventId - (uint8)EVENT_TYPE::SWITCH_ONE_START_EVENT]._IsActive &&
			!targetRoom._switchs[eventId - (uint8)EVENT_TYPE::SWITCH_ONE_START_EVENT]._IsOnInteraction) // ������ ��ȣ�ۿ��� ������ ���
		{

			// ���� �� ��ȣ�ۿ� ���·� ����
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
	case EVENT_TYPE::SWITCH_ONE_END_EVENT: // ��ȣ�ۿ� ���߿� ���� ���
	case EVENT_TYPE::SWITCH_TWO_END_EVENT: // ��ȣ�ۿ� ���߿� ���� ���
	case EVENT_TYPE::SWITCH_THREE_END_EVENT: // ��ȣ�ۿ� ���߿� ���� ���
	{
		if (!targetRoom._switchs[eventId - (uint8)EVENT_TYPE::SWITCH_ONE_END_EVENT]._IsActive &&
			targetRoom._switchs[eventId -  (uint8)EVENT_TYPE::SWITCH_ONE_END_EVENT]._IsOnInteraction) // ������ ��ȣ�ۿ��� ���߿� �߻��� ���
		{
			std::cout << "switch[" << eventId - (uint8)EVENT_TYPE::SWITCH_ONE_END_EVENT << "] " << "Cancel \n";
			// ��ȣ�ۿ� ���·� ����
			targetRoom._switchs[eventId - 5].SwitchInteractionOn(false);
			targetRoom._switchs[eventId - 5].ResetGuage(); // ������ ������ �ʱ�ȭ
			SC_EVENTPACKET packet;
			packet.size = sizeof(SC_EVENTPACKET);
			packet.type = (uint8)SC_PACKET_TYPE::GAMEEVENT;
			packet.eventId = eventId; // 0: ������ ���� // ������ ���� // 1: ������ �Ϸ� // 2: ����� ���� ó�� // 3: ����� ���� ��Ÿ��

			S2C_SWITCH_ANIM packet2;
			packet2.size = sizeof(S2C_SWITCH_ANIM);
			packet2.type = (uint8)S_PACKET_TYPE::SWITCH_ANIM_CANCEL;
			packet2.idx = targetRoom.GetMyPlayerFromRoom(_sid).m_idx;
			targetRoom.BroadCastingExcept(&packet2, _sid);
			targetRoom.BroadCastingExcept(&packet, _sid);
		}
	}
	break;
	// ============== ���� ���� �̺�Ʈ ================
	case EVENT_TYPE::ATTACK_EVENT:
	{
		// ���� �̺�Ʈ ����
		int32 myIdx = targetRoom.GetMyPlayerFromRoom(_sid).m_idx;
		PlayerInfo& bossPlayer = targetRoom.GetMyPlayerFromRoom(_sid);
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

