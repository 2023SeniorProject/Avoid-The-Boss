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
		//============= 스위치 관련 이벤트 ===================
	case EVENT_TYPE::SWITCH_ONE_START_EVENT:
	case EVENT_TYPE::SWITCH_TWO_START_EVENT:
	case EVENT_TYPE::SWITCH_THREE_START_EVENT:
	{
		SGenerator& targetGen = gm.GetGeneratorByIdx(eventId - (uint8)EVENT_TYPE::SWITCH_ONE_START_EVENT);
		if (!targetGen._IsActive && !targetGen._IsOnInteraction) // 발전기 상호작용이 가능할 경우
		{
			// 검증 후 상호작용 상태로 변경
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
	case EVENT_TYPE::SWITCH_ONE_END_EVENT: // 상호작용 도중에 끝낸 경우
	case EVENT_TYPE::SWITCH_TWO_END_EVENT: // 상호작용 도중에 끝낸 경우
	case EVENT_TYPE::SWITCH_THREE_END_EVENT: // 상호작용 도중에 끝낸 경우
	{
		SGenerator& targetGen = gm.GetGeneratorByIdx(eventId - (uint8)EVENT_TYPE::SWITCH_ONE_END_EVENT);
		if (!targetGen._IsActive && targetGen._IsOnInteraction) // 발전기 상호작용이 도중에 발생한 경우
		{
	
			// 상호작용 상태로 변경
			targetGen.GenInteractionOn(false);
			SC_EVENTPACKET packet;
			packet.size = sizeof(SC_EVENTPACKET);
			packet.type = (uint8)SC_GAME_PACKET_TYPE::GAMEEVENT;
			packet.eventId = eventId; // 0: 발전기 시작 // 발전기 종료 // 1: 발전기 완료 // 2: 사장님 공격 처리 // 3: 사장님 공격 쿨타임

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

	case EVENT_TYPE::SWITCH_ONE_ACTIVATE_EVENT: // 상호작용 도중에 끝낸 경우
	case EVENT_TYPE::SWITCH_TWO_ACTIVATE_EVENT: // 상호작용 도중에 끝낸 경우
	case EVENT_TYPE::SWITCH_THREE_ACTIVATE_EVENT: // 상호작용 도중에 끝낸 경우
	{
		
		SGenerator& targetGen = gm.GetGeneratorByIdx(eventId - (uint8)EVENT_TYPE::SWITCH_ONE_ACTIVATE_EVENT);
		targetGen.GenActivate(true);
		SC_EVENTPACKET packet;
		packet.size = sizeof(SC_EVENTPACKET);
		packet.type = (uint8)SC_GAME_PACKET_TYPE::GAMEEVENT;
		packet.eventId = eventId;
		targetRoom.BroadCasting(&packet);
		// 일단 활성화 사실 알리기


	}
	break;
	
	// ============== 깨우기 관련 이벤트 =============
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
