#include "pch.h"
#include "packetEvent.h"
#include "CGameManager.h"
void InteractionEvent::Task()
{
	int16 roomNum = ServerIocpCore._clients[_sid]->_myRm;
	Room& targetRoom = ServerIocpCore._rmgr->GetRoom(roomNum);
	CGameManager& gm = targetRoom._gameLogic;
	switch ((EVENT_TYPE)eventId)
	{
		//============= ����ġ ���� �̺�Ʈ ===================
	case EVENT_TYPE::SWITCH_ONE_START_EVENT:
	case EVENT_TYPE::SWITCH_TWO_START_EVENT:
	case EVENT_TYPE::SWITCH_THREE_START_EVENT:
	{
		SGenerator& targetGen = gm.GetGeneratorByIdx(eventId - (uint8)EVENT_TYPE::SWITCH_ONE_START_EVENT);
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
				packet.type = (uint8)SC_GAME_PACKET_TYPE::GAMEEVENT;
				packet.eventId = eventId;

				S2C_ANIMPACKET animPacket;
				animPacket.size = sizeof(S2C_ANIMPACKET);
				animPacket.type = (uint8)S_GAME_PACKET_TYPE::ANIM;
				animPacket.idx =  gm.GetPlayerBySid(_sid).m_idx;
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
		SGenerator& targetGen = gm.GetGeneratorByIdx(eventId - (uint8)EVENT_TYPE::SWITCH_ONE_END_EVENT);
		if (!targetGen._IsActive && targetGen._IsOnInteraction) // ������ ��ȣ�ۿ��� ���߿� �߻��� ���
		{
	
			// ��ȣ�ۿ� ���·� ����
			targetGen.GenInteractionOn(false);
			SC_EVENTPACKET packet;
			packet.size = sizeof(SC_EVENTPACKET);
			packet.type = (uint8)SC_GAME_PACKET_TYPE::GAMEEVENT;
			packet.eventId = eventId; // 0: ������ ���� // ������ ���� // 1: ������ �Ϸ� // 2: ����� ���� ó�� // 3: ����� ���� ��Ÿ��

			S2C_ANIMPACKET animPacket;
			animPacket.size = sizeof(S2C_ANIMPACKET);
			animPacket.type = (uint8)S_GAME_PACKET_TYPE::ANIM;
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
		
		SGenerator& targetGen = gm.GetGeneratorByIdx(eventId - (uint8)EVENT_TYPE::SWITCH_ONE_ACTIVATE_EVENT);
		targetGen.GenActivate(true);
		SC_EVENTPACKET packet;
		packet.size = sizeof(SC_EVENTPACKET);
		packet.type = (uint8)SC_GAME_PACKET_TYPE::GAMEEVENT;
		packet.eventId = eventId;
		targetRoom.BroadCasting(&packet);
		// �ϴ� Ȱ��ȭ ��� �˸���


	}
	break;
	
	// ============== ����� ���� �̺�Ʈ =============
	case EVENT_TYPE::RESCUE_PLAYER_ONE:
	case EVENT_TYPE::RESCUE_PLAYER_TWO:
	case EVENT_TYPE::RESCUE_PLAYER_THREE:
	case EVENT_TYPE::RESCUE_PLAYER_FOUR:
	{
		SPlayer& p = gm.GetPlayerByIdx((int8)eventId - (int8)EVENT_TYPE::RESCUE_PLAYER_ONE);
		if (!p.m_bIsRescue)
		{
			p.m_bIsRescue = true;
			SC_EVENTPACKET packet;
			packet.type = (uint8)SC_GAME_PACKET_TYPE::GAMEEVENT;
			packet.size = sizeof(SC_EVENTPACKET);
			packet.eventId = eventId;
			targetRoom.BroadCastingExcept(&packet, _sid);
		}

	}
	break;
	case EVENT_TYPE::RESCUE_CANCEL_PLAYER_ONE:
	case EVENT_TYPE::RESCUE_CANCEL_PLAYER_TWO:
	case EVENT_TYPE::RESCUE_CANCEL_PLAYER_THREE:
	case EVENT_TYPE::RESCUE_CANCEL_PLAYER_FOUR:
	{
		SPlayer& p = gm.GetPlayerByIdx((int8)eventId - (int8)EVENT_TYPE::RESCUE_CANCEL_PLAYER_ONE);
		if (p.m_bIsRescue) p.m_bIsRescue = false;
		std::cout << "RESCUE CANCEL\n";

		SC_EVENTPACKET packet;
		packet.type = (uint8)SC_GAME_PACKET_TYPE::GAMEEVENT;
		packet.size = sizeof(SC_EVENTPACKET);
		packet.eventId = eventId;
		targetRoom.BroadCastingExcept(&packet,_sid);

	}
	break;
	case EVENT_TYPE::ALIVE_PLAYER_ONE:
	case EVENT_TYPE::ALIVE_PLAYER_TWO:
	case EVENT_TYPE::ALIVE_PLAYER_THREE:
	case EVENT_TYPE::ALIVE_PLAYER_FOUR:
	{
		SPlayer& p = gm.GetPlayerByIdx((int8)eventId - (int8)EVENT_TYPE::ALIVE_PLAYER_ONE);
		p.ProcessAlive();
	}
	break;
	case EVENT_TYPE::EXIT_PLAYER_ONE:
	case EVENT_TYPE::EXIT_PLAYER_TWO:
	case EVENT_TYPE::EXIT_PLAYER_THREE:
	case EVENT_TYPE::EXIT_PLAYER_FOUR:
	{
		int32 playerIdx = eventId - (int8)EVENT_TYPE::EXIT_PLAYER_ONE;
		SPlayer& p = gm.GetPlayerByIdx((int8)eventId - (int8)EVENT_TYPE::ALIVE_PLAYER_ONE);
		if(!p.m_isEscaped) p.m_isEscaped = true;

	}
	default:
		std::cout << "UnKnown Game Event Please Check Your Packet Type\n";
		break;
	}
};

void moveEvent::Task()
{
	// to do move Player in gameLogic
	int16 roomNum = ServerIocpCore._clients[_sid]->_myRm;
	CGameManager& gm = ServerIocpCore._rmgr->GetRoom(roomNum)._gameLogic;
	Room& targetRoom = ServerIocpCore._rmgr->GetRoom(roomNum);
	SPlayer& targetPlayer = gm.GetPlayerBySid(_sid);
	
	targetPlayer.SetDirection(_dir);
	
	if (targetPlayer.m_idx == 0) targetPlayer.Move(_key, BOSS_VELOCITY);
	else targetPlayer.Move(_key, EMPLOYEE_VELOCITY);

	if (_key == 0)
	{
		S2C_POS packet;
		packet.sid = _sid;
		packet.size = sizeof(S2C_POS);
		packet.type = (uint8)S_GAME_PACKET_TYPE::SPOS;
		packet.x = targetPlayer.GetPosition().x;
		packet.z = targetPlayer.GetPosition().z;
		
		targetRoom.BroadCastingExcept(&packet, _sid);
	}
	
};

void AttackEvent::Task()
{
	int16 roomNum = ServerIocpCore._clients[_sid]->_myRm;

	CGameManager& gm = ServerIocpCore._rmgr->GetRoom(roomNum)._gameLogic;
	bool retVal = ServerIocpCore._rmgr->GetRoom(roomNum).ProcessAttackEvent(_wf, _tidx);
	SPlayer& emp = gm.GetPlayerByIdx(_tidx);
	
	
	
	if (retVal)
	{
		emp.ProcessAttack();

		S2C_ANIMPACKET apacket;
		apacket.size = sizeof(S2C_ANIMPACKET);
		apacket.type = (uint8)S_GAME_PACKET_TYPE::ANIM;
		apacket.track = 6;
		ServerIocpCore._rmgr->GetRoom(roomNum).BroadCastingExcept(&apacket, _sid);

		SC_EVENTPACKET epacket;
		epacket.size = sizeof(SC_EVENTPACKET);
		epacket.type = (uint8)SC_GAME_PACKET_TYPE::GAMEEVENT;
		epacket.eventId = (int32)EVENT_TYPE::ATTACKED_PLAYER_ONE + (int32)_tidx;

		ServerIocpCore._rmgr->GetRoom(roomNum).BroadCasting(&epacket);
	

	}
}
