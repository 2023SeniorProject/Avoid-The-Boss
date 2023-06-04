#include "pch.h"
#include "ClientPacketEvent.h"
#include "GameFramework.h"
#include "CBoss.h"
#include "CBullet.h"

void InteractionEvent::Task()
{

	switch ((EVENT_TYPE)eventId)
	{
	case EVENT_TYPE::SWITCH_ONE_START_EVENT:
	case EVENT_TYPE::SWITCH_TWO_START_EVENT:
	case EVENT_TYPE::SWITCH_THREE_START_EVENT:
	{
		CGenerator* targetGen = mainGame.m_pScene->GetSceneGenerator(eventId - (uint8)EVENT_TYPE::SWITCH_ONE_START_EVENT);
		if (targetGen == nullptr) break;
		targetGen->SetAlreadyOn(true);
		targetGen->SetAnimationCount(BUTTON_ANIM_FRAME);
	}
	break;
	case EVENT_TYPE::SWITCH_ONE_END_EVENT:
	case EVENT_TYPE::SWITCH_TWO_END_EVENT:
	case EVENT_TYPE::SWITCH_THREE_END_EVENT:
	{
		std::cout << "Switch Cancel\n";
		CGenerator* targetGen = mainGame.m_pScene->GetSceneGenerator(eventId - (uint8)EVENT_TYPE::SWITCH_ONE_START_EVENT);
		if (targetGen == nullptr) break;
		targetGen->SetAlreadyOn(false);
		targetGen->SetAnimationCount(0);
	}
	break;
	// ���� ����ġ Ȱ��ȭ�� �ƴٴ� ��Ŷ�� ���� �Ǿ��� ��,
	case EVENT_TYPE::SWITCH_ONE_ACTIVATE_EVENT:
	case EVENT_TYPE::SWITCH_TWO_ACTIVATE_EVENT:
	case EVENT_TYPE::SWITCH_THREE_ACTIVATE_EVENT:
	{
		CGenerator* targetGen = mainGame.m_pScene->GetSceneGenerator(eventId - (uint8)EVENT_TYPE::SWITCH_ONE_START_EVENT);
		if (targetGen == nullptr) break;
		targetGen->m_bGenActive = true;

		mainGame.m_pScene->m_ActiveGeneratorCnt.fetch_add(1); // ī��Ʈ ����
		if (mainGame.m_pScene->m_ActiveGeneratorCnt == 1) mainGame.m_pScene->m_bEmpExit = true; // Ż�� ���� true
		
	}
	break;
	case EVENT_TYPE::HIDE_PLAYER_ONE:
	case EVENT_TYPE::HIDE_PLAYER_TWO:
	case EVENT_TYPE::HIDE_PLAYER_THREE:
	case EVENT_TYPE::HIDE_PLAYER_FOUR:
	{
		std::cout << "PLAYER_HIDE\n";
		CPlayer* player = mainGame.m_pScene->_players[eventId - (uint8)EVENT_TYPE::HIDE_PLAYER_ONE];
		if (player == nullptr) break;
		player->m_hide = true;
	}
	break;
	case EVENT_TYPE::ATTACK_EVENT:
	{	
		CBoss* boss = static_cast<CBoss*>(mainGame.m_pScene->_players[0]);
		if (boss == nullptr) break;
		boss->SetnInteractionCountTime(BOSS_INTERACTION_TIME);
		boss->SetAttackAnimOtherClient();
		
	}
		break;
	case EVENT_TYPE::ATTACKED_PLAYER_TWO:
	case EVENT_TYPE::ATTACKED_PLAYER_THREE:
	case EVENT_TYPE::ATTACKED_PLAYER_FOUR:
		// ========= �÷��̾� �ǰ� ���� �ִϸ��̼� ���
		// ========= �÷��̾� HP ���� ================
	{
		
		CPlayer* player = mainGame.m_pScene->_players[eventId - (int8)(EVENT_TYPE::ATTACKED_PLAYER_ONE)];
		if (player == nullptr) break;
		static_cast<CEmployee*>(player)->PlayerAttacked();
	}
		break;
	case EVENT_TYPE::ALIVE_PLAYER_TWO:
	case EVENT_TYPE::ALIVE_PLAYER_THREE:
	case EVENT_TYPE::ALIVE_PLAYER_FOUR:
		// ========= �÷��̾� �ǰ� ���� �ִϸ��̼� ���
		// ========= �÷��̾� HP ���� ================
	{
		CPlayer* player = mainGame.m_pScene->_players[eventId - (int8)(EVENT_TYPE::ALIVE_PLAYER_ONE)];
		if (player == nullptr) break;
		static_cast<CEmployee*>(player)->SetBehavior(PLAYER_BEHAVIOR::STAND);
	}
	break;
	default:
		break;
	}
};

void moveEvent::Task()
{
		player->SetDirection(_dir);
		if(player->m_ctype == (uint8)PLAYER_TYPE::BOSS) static_cast<CBoss*>(player)->Move(_key, BOSS_VELOCITY);
		else static_cast<CEmployee*>(player)->Move(_key, EMPLOYEE_VELOCITY);
}

void posEvent::Task()
{
	XMFLOAT3 curPos = player->GetPosition();
	XMFLOAT3 distance = Vector3::Subtract(curPos, _pos);
	if (Vector3::Length(distance) > 0.2f)
	{
		std::cout << "Mass Offset Detected. Reseting Pos\n";
		player->MakePosition(XMFLOAT3(_pos.x, _pos.y, _pos.z));
	}
}
