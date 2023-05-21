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
		CGenerator* mSwitch = mainGame.m_pScene->m_ppSwitches[eventId - 2];
		mSwitch->m_lock.lock();
		mSwitch->m_bOtherPlayerInteractionOn = true;
		mainGame.m_pScene->m_ppSwitches[eventId - 2]->InteractAnimation(true); // ������ �ִϸ��̼� ����� �����Ѵ�.
		mainGame.m_pScene->m_ppSwitches[eventId - 2]->SetAnimationCount(BUTTON_ANIM_FRAME);
		mSwitch->m_lock.unlock();
	}
	break;
	case EVENT_TYPE::SWITCH_ONE_END_EVENT:
	case EVENT_TYPE::SWITCH_TWO_END_EVENT:
	case EVENT_TYPE::SWITCH_THREE_END_EVENT:
	{
		std::cout << "Switch Cancel\n";
		CGenerator* mSwitch = mainGame.m_pScene->m_ppSwitches[eventId - 5];
		mSwitch->m_lock.lock();
		mSwitch->m_bOtherPlayerInteractionOn = false;
		mSwitch->m_lock.unlock();
	}
	break;
	// ���� ����ġ Ȱ��ȭ�� �ƴٴ� ��Ŷ�� ���� �Ǿ��� ��,
	case EVENT_TYPE::SWITCH_ONE_ACTIVATE_EVENT:
	case EVENT_TYPE::SWITCH_TWO_ACTIVATE_EVENT:
	case EVENT_TYPE::SWITCH_THREE_ACTIVATE_EVENT:
	{
		CGenerator* mSwitch = mainGame.m_pScene->m_ppSwitches[eventId - 8];
		mSwitch->m_lock.lock();
		mainGame.m_pScene->m_ppSwitches[eventId - 8]->m_bSwitchActive = true;
		mSwitch->m_lock.unlock();
		mainGame.m_pScene->m_ActiveSwitchCnt.fetch_add(1);
		std::cout << (int)(eventId - 8) << "Switch Activate\n";
		if (mainGame.m_pScene->m_ActiveSwitchCnt == 1) // ���� 3���� ����ġ�� ��� Ȱ��ȭ �Ǿ��ٸ�, 
		{
			std::cout << "Clear\n";
			mainGame.m_pScene->m_bIsExitReady = true; // Ż�� ���� true
		}
	}
	break;
	case EVENT_TYPE::HIDE_PLAYER_ONE:
	case EVENT_TYPE::HIDE_PLAYER_TWO:
	case EVENT_TYPE::HIDE_PLAYER_THREE:
	case EVENT_TYPE::HIDE_PLAYER_FOUR:
	{
		std::cout << "PLAYER_HIDE\n";
		mainGame.m_pScene->_players[eventId - (uint8)EVENT_TYPE::HIDE_PLAYER_ONE]->m_hide = true;
	}
	break;
	case EVENT_TYPE::ATTACK_EVENT:
		player->SetInteractionAnimation(true);
		player->m_InteractionCountTime = BOSS_INTERACTION_TIME;
		((CBoss*)player)->m_pBullet->SetOnShoot(true);
		((CBoss*)player)->AttackAnimationOn();
		break;
	default:
		break;
	}
};

void moveEvent::Task()
{

		player->SetDirection(_dir);
		player->Move(_key, PLAYER_VELOCITY);

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
