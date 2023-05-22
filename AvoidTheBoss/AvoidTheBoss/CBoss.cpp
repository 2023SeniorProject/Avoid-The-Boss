#include "pch.h"
#include "CBoss.h"
#include "CBullet.h"
#include "clientIocpCore.h"
#include "InputManager.h"


CBoss::CBoss(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_type = 0;
	m_pCamera = ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);
	m_ctype = (uint8)PLAYER_TYPE::BOSS;
	m_nCharacterType = CHARACTER_TYPE::BOSS;

	CLoadedModelInfo* pBossModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, g_pstrCharactorRefernece[(int)m_nCharacterType], NULL, Layout::PLAYER);
	SetChild(pBossModel->m_pModelRootObject, true);

	m_pSkinnedAnimationController = new CAnimationController(pd3dDevice, pd3dCommandList, 4, pBossModel);
	m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);//Idle
	m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);//Run
	m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2);//Shoot
	m_pSkinnedAnimationController->SetTrackAnimationSet(3, 3);//RunningShoot

	//m_pSkinnedAnimationController->SetTrackSpeed(3, 0.83f);
	m_pSkinnedAnimationController->SetTrackSpeed(2, 0.5f);

	m_pSkinnedAnimationController->SetTrackEnable(0, true);
	m_pSkinnedAnimationController->SetTrackEnable(1, false);
	m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_pSkinnedAnimationController->SetTrackEnable(3, false);

	

	SetScale(XMFLOAT3(1.f, 1.f, 1.f));
	SetPosition(XMFLOAT3(0.0f, 0.25f, -30.0f));

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	if (pBossModel) delete pBossModel;
}

CBoss::~CBoss()
{
}

CCamera* CBoss::ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed)
{
	DWORD nCurrentCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00;
	if (nCurrentCameraMode == nNewCameraMode)
		return(m_pCamera);

	float MaxDepthofMap = 5000.0f;//sqrt(2) * 50 * UNIT + 2 * UNIT;
	switch (nNewCameraMode)
	{
	case FIRST_PERSON_CAMERA:
		m_pCamera = OnChangeCamera(FIRST_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 1.4f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, MaxDepthofMap, ASPECT_RATIO, 60.0f); //5000.f
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	case THIRD_PERSON_CAMERA:
		m_pCamera = OnChangeCamera(THIRD_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 1.7f * UNIT, -5 * UNIT));
		m_pCamera->GenerateProjectionMatrix(1.01f, MaxDepthofMap, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	default:
		break;
	}
	m_pCamera->SetPosition(Vector3::Add(m_xmf3Position, m_pCamera->GetOffset()));

	Update(fTimeElapsed, CLIENT_TYPE::NONE);

	return(m_pCamera);
}



void CBoss::Rotate(float x, float y, float z)
{
	CPlayer::Rotate(x, y, z);
	m_pBullet->Rotate(x, y, z);
}

void CBoss::PrepareAnimate()
{
}

void CBoss::Move(const int16& dwDirection, float fDistance)
{

	if (LOBYTE(dwDirection)) // 1. 캐릭터 이동량이 있을 때 (WASD 키 입력)
	{
		if (!m_OnInteraction) // 공격 키 미입력, 이동키 입력 --> 달리기
		{
			m_behavior = RUN;
		}
		else // 공격 키, 이동키 모두 입력 --> 달리면서 쏘기
		{
			m_behavior = RUN_ATTACK;
			m_pBullet->SetOnShoot(true);
		
		}
	}
	else if (!LOBYTE(dwDirection)) // 이동키 입력이 아닐 때
	{
		if (!m_OnInteraction) // 공격 키, 이동키 모두 미입력
		{
			m_behavior = IDLE;
		}
		else // 공격 키만 입력, 이동키는 미입력
		{
			m_behavior = ATTACK;
			if (m_pBullet) m_pBullet->SetOnShoot(true);
		}
	}

	CPlayer::Move(dwDirection, BOSS_VELOCITY);
  
}

void CBoss::SetAttackAnimOtherClient()
{
	if (m_pSkinnedAnimationController == nullptr) return;
	if (!Vector3::IsZero(m_xmf3Velocity))
	{
			if (m_InteractionCountTime == BOSS_INTERACTION_TIME)
			{
				SetRunAttackAnimTrack();
			}
			else if (m_InteractionCountTime < BOSS_INTERACTION_TIME)
			{
				if (m_InteractionCountTime <= 0) 
				{
					m_OnInteraction = false;
					m_pBullet->SetOnShoot(false);
					SetRunAnimTrack();
				}
				m_InteractionCountTime -= 1;
			}
	}
	else if(Vector3::IsZero(m_xmf3Velocity))
	{
		if (m_InteractionCountTime == BOSS_INTERACTION_TIME)
		{
			SetAttackAnimTrack();
		}
		else if (m_InteractionCountTime < BOSS_INTERACTION_TIME)
		{
			if (m_InteractionCountTime <= 0)
			{
				m_OnInteraction = false;
				m_pBullet->SetOnShoot(false);
				SetIdleAnimTrack();
			}
			m_InteractionCountTime -= 1;
		}
	}
}

void CBoss::Update(float fTimeElapsed, CLIENT_TYPE ptype)
{
	CPlayer::Update(fTimeElapsed, ptype);

	
	if (m_pBullet)
	{
		m_pBullet->SetBulletPosition(GetPosition());
		m_pBullet->Update(fTimeElapsed);
	}
	
	LateUpdate(fTimeElapsed, ptype);
	
}

void CBoss::LateUpdate(float fTimeElapsed, CLIENT_TYPE ptype)
{
	if (ptype == CLIENT_TYPE::OWNER) m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	else if (m_pBullet) SetAttackAnimOtherClient();
	AnimTrackUpdate();
}



void CBoss::SetIdleAnimTrack()
{
	if (m_pSkinnedAnimationController == nullptr) return;
	m_pSkinnedAnimationController->SetTrackEnable(0, true); // 아이들
	m_pSkinnedAnimationController->SetTrackEnable(1, false);
	m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_pSkinnedAnimationController->SetTrackEnable(3, false);

	m_pSkinnedAnimationController->SetTrackPosition(0, 0.0f);
	m_pSkinnedAnimationController->SetTrackPosition(1, 0.0f);
	m_pSkinnedAnimationController->SetTrackPosition(2, 0.0f);
	m_pSkinnedAnimationController->SetTrackPosition(3, 0.0f);
}

void CBoss::SetRunAnimTrack()
{
	if (m_pSkinnedAnimationController == nullptr) return;
	m_pSkinnedAnimationController->SetTrackEnable(0, false);
	m_pSkinnedAnimationController->SetTrackEnable(1, true); // 달리기
	m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_pSkinnedAnimationController->SetTrackEnable(3, false);

	m_pSkinnedAnimationController->SetTrackPosition(0, 0.0f);
	m_pSkinnedAnimationController->SetTrackPosition(1, 0.0f);
	m_pSkinnedAnimationController->SetTrackPosition(2, 0.0f);
	m_pSkinnedAnimationController->SetTrackPosition(3, 0.0f);
}

void CBoss::SetAttackAnimTrack()
{
	if (m_pSkinnedAnimationController == nullptr) return;
	m_pSkinnedAnimationController->SetTrackEnable(0, false);
	m_pSkinnedAnimationController->SetTrackEnable(1, false);
	m_pSkinnedAnimationController->SetTrackEnable(2, true); // 공격
	m_pSkinnedAnimationController->SetTrackEnable(3, false);

	m_pSkinnedAnimationController->SetTrackPosition(0, 0.0f);
	m_pSkinnedAnimationController->SetTrackPosition(1, 0.0f);
	m_pSkinnedAnimationController->SetTrackPosition(2, 0.0f);
	m_pSkinnedAnimationController->SetTrackPosition(3, 0.0f);
}

void CBoss::SetRunAttackAnimTrack()
{
	if (m_pSkinnedAnimationController == nullptr) return;
	m_pSkinnedAnimationController->SetTrackEnable(0, false);
	m_pSkinnedAnimationController->SetTrackEnable(1, false);
	m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_pSkinnedAnimationController->SetTrackEnable(3, true);

	m_pSkinnedAnimationController->SetTrackPosition(0, 0.0f);
	m_pSkinnedAnimationController->SetTrackPosition(1, 0.0f);
	m_pSkinnedAnimationController->SetTrackPosition(2, 0.0f);
	m_pSkinnedAnimationController->SetTrackPosition(3, 0.0f);

	m_InteractionCountTime -= 1;
}

void CBoss::AnimTrackUpdate()
{
	switch (m_behavior)
	{
		case IDLE:
			SetIdleAnimTrack();
			break;
		case RUN:
			SetRunAnimTrack();
			break;
		case ATTACK:
		case RUN_ATTACK:
		{
			if (m_InteractionCountTime == BOSS_INTERACTION_TIME)
			{
				if (m_behavior == ATTACK) SetRunAttackAnimTrack();
				else if (m_behavior == RUN_ATTACK) SetRunAttackAnimTrack();
			}
			else if (m_InteractionCountTime < BOSS_INTERACTION_TIME)
			{
				if (m_InteractionCountTime <= 0)
				{
					m_OnInteraction = false;
					SetRunAnimTrack();
				}
				m_InteractionCountTime -= 1;
			}
		}
		break;
	}

}


void CBoss::ProcessInput(const int16& dwDirection)
{

	// 1. 공격 키를 눌렀을 경우 처리 
	if (dwDirection & KEY_SPACE)
	{
		if (m_InteractionCountTime <= 0 && m_OnInteraction == false)
		{
			//보스 캐릭터 애니메이션 처리
			SetInteractionAnimation(true);
			m_InteractionCountTime = BOSS_INTERACTION_TIME;
			// 05-06 공격 시, 사장님 공격 이벤트 전송
			if (InputManager::GetInstance().GetKeyBuffer(KEY_TYPE::SPACE) == (uint8)KEY_STATUS::KEY_PRESS)
			{
				SC_EVENTPACKET packet;
				packet.eventId = (uint8)EVENT_TYPE::ATTACK_EVENT;
				packet.type = SC_PACKET_TYPE::GAMEEVENT;
				packet.size = sizeof(SC_EVENTPACKET);
				clientCore._client->DoSend(&packet);
			}
		}
	}

	Move(dwDirection, BOSS_VELOCITY);
}

