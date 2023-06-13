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

	//CLoadedModelInfo* pBossModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, g_pstrCharactorRefernece[(int)m_nCharacterType], NULL, Layout::PLAYER);
	//SetChild(pBossModel->m_pModelRootObject, true);

	CLoadedModelInfo* pBossUpperModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Boss_Shooting_Run_UpperBody5.bin", NULL, Layout::PLAYER);
	SetChild(pBossUpperModel->m_pModelRootObject, true);
	
	CLoadedModelInfo* pBossLowerModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Boss_Shooting_Run_LowerBody(3).bin", NULL, Layout::PLAYER);
	SetChild(pBossLowerModel->m_pModelRootObject, true);
		
	m_pSkinnedAnimationController = new CAnimationController(pd3dDevice, pd3dCommandList, 4, pBossUpperModel);
	m_pSkinnedAnimationController1 = new CAnimationController(pd3dDevice, pd3dCommandList, 4, pBossLowerModel);

	m_pSkinnedAnimationController->SetTrackAnimationSet(3, 0);//RunningShoot 
	m_pSkinnedAnimationController->SetTrackAnimationSet(0, 1);//Idle
	m_pSkinnedAnimationController->SetTrackAnimationSet(1, 2);//Run
	m_pSkinnedAnimationController->SetTrackAnimationSet(2, 3);//Shoot

	m_pSkinnedAnimationController1->SetTrackAnimationSet(1, 0);//Run
	m_pSkinnedAnimationController1->SetTrackAnimationSet(0, 1);//Idle
	m_pSkinnedAnimationController1->SetTrackAnimationSet(2, 2);//Run
	m_pSkinnedAnimationController1->SetTrackAnimationSet(3, 3);//Run

	m_pSkinnedAnimationController->SetTrackEnable(0, true);
	m_pSkinnedAnimationController1->SetTrackEnable(0, true);

	m_pSkinnedAnimationController1->SetTrackEnable(2, false);
	m_pSkinnedAnimationController1->SetTrackEnable(3, false);

	//m_pSkinnedAnimationController = new CAnimationController(pd3dDevice, pd3dCommandList, 5, pBossModel);


	////m_pSkinnedAnimationController->SetTrackSpeed(3, 0.83f);
	//m_pSkinnedAnimationController->SetTrackSpeed(2, 0.5f);

	//m_pSkinnedAnimationController->SetTrackEnable(0, true);
	//m_pSkinnedAnimationController->SetTrackEnable(1, false);
	//m_pSkinnedAnimationController->SetTrackEnable(2, false);
	//m_pSkinnedAnimationController->SetTrackEnable(3, false);
	//m_pSkinnedAnimationController->SetTrackEnable(4, false);


	//	m_pSkinnedAnimationController->SetCallbackKeys(1, 2);
	//#ifdef _WITH_SOUND_RESOURCE
	//	m_pSkinnedAnimationController->SetCallbackKey(0, 0.1f, _T("Footstep01"));
	//	m_pSkinnedAnimationController->SetCallbackKey(1, 0.5f, _T("Footstep02"));
	//	m_pSkinnedAnimationController->SetCallbackKey(2, 0.9f, _T("Footstep03"));
	//#else
	//	m_pSkinnedAnimationController->SetCallbackKey(1, 0, 0.001f, _T("Sound/Footstep01.wav"));
	//	m_pSkinnedAnimationController->SetCallbackKey(1, 1, 0.125f, _T("Sound/Footstep02.wav"));
	//	m_pSkinnedAnimationController->SetCallbackKey(1, 2, 0.39f, _T("Sound/Footstep03.wav"));
	//#endif
		//CAnimationCallbackHandler *pAnimationCallbackHandler = new CSoundCallbackHandler();
		//m_pSkinnedAnimationController->SetAnimationCallbackHandler(1, pAnimationCallbackHandler);

	//SetPlayerUpdatedContext();
	//SetCameraUpdatedContext();

	SetScale(XMFLOAT3(1.f, 1.f, 1.f));
	SetPosition(XMFLOAT3(0.0f, 0.25f, -30.0f));
	//CGameObject::Rotate(0.0f, -180.0f, 0.0f);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	//if (pBossModel) delete pBossModel;

	if (pBossUpperModel) delete pBossUpperModel;
	if (pBossLowerModel) delete pBossLowerModel;

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
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 1.7f * UNIT, 5 * UNIT));
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
	CPlayer::Move(dwDirection, BOSS_VELOCITY);
}

void CBoss::SetAttackAnimOtherClient()
{
	// 스키닝 애니메이션이 진행중이거나 이미 애니메이션 트랙이 재생 중일 경우
	if (m_pSkinnedAnimationController == nullptr || m_behavior == (int32)PLAYER_BEHAVIOR::ATTACK || m_behavior == (int32)PLAYER_BEHAVIOR::RUN_ATTACK) return;
	if (!Vector3::IsZero(m_xmf3Velocity)) //*문제 코드 : 공격 전환에 대해서는 속도기준으로 정하면 안된다. 쿨타임 계산만 해야함.
	{
			//m_behavior = (int32)PLAYER_BEHAVIOR::RUN_ATTACK;
			//m_InteractionCountTime = BOSS_INTERACTION_TIME;
	}
	else if(Vector3::IsZero(m_xmf3Velocity))
	{
		//m_behavior == (int32)PLAYER_BEHAVIOR::ATTACK;
		//m_InteractionCountTime = BOSS_INTERACTION_TIME;
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

	AnimationLogicUpdate(); // 쿨타임 계산

	AimationStateUpdate(); // 애니메이션 트랙 상태 결정

	AnimTrackUpdate(); // 애니메이션 트랙 상태 변경

	LateUpdate(fTimeElapsed, ptype);
}

void CBoss::LateUpdate(float fTimeElapsed, CLIENT_TYPE ptype)
{
	if (ptype == CLIENT_TYPE::OWNER) m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	else if (ptype != CLIENT_TYPE::OTHER_PLAYER && m_pBullet) SetAttackAnimOtherClient();
	//AnimTrackUpdate();
}


void CBoss::AnimationLogicUpdate()
{
	if (m_bOnInteraction)
	{
		if (m_InteractionCountTime >= BOSS_INTERACTION_TIME )
		{
			m_bOnInteraction = false;
			m_InteractionCountTime = 0;
			return;
		}
		m_InteractionCountTime++;
	}
}

void CBoss::AimationStateUpdate()
{
	if (m_bOnInteraction)
	{
		if(Vector3::IsZero(m_xmf3Velocity))
			m_behavior = (int32)PLAYER_BEHAVIOR::ATTACK;
		else
			m_behavior = (int32)PLAYER_BEHAVIOR::RUN_ATTACK;
	}
	else
	{
		if (Vector3::IsZero(m_xmf3Velocity))
			m_behavior = (int32)PLAYER_BEHAVIOR::IDLE;
		else
			m_behavior = (int32)PLAYER_BEHAVIOR::RUN;
	}
}

void CBoss::SetIdleAnimTrack()
{
	if (m_pSkinnedAnimationController == nullptr || m_pSkinnedAnimationController1 == nullptr) return;
	m_pSkinnedAnimationController->SetTrackEnable(0, true); // 아이들
	m_pSkinnedAnimationController->SetTrackEnable(1, false);
	m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_pSkinnedAnimationController->SetTrackEnable(3, false);

	m_pSkinnedAnimationController->SetTrackPosition(0, 0.0f);
	m_pSkinnedAnimationController->SetTrackPosition(1, 0.0f);
	m_pSkinnedAnimationController->SetTrackPosition(2, 0.0f);
	m_pSkinnedAnimationController->SetTrackPosition(3, 0.0f);

	// ===============  하체 ===========================
	m_pSkinnedAnimationController1->SetTrackEnable(0, true); // IDLE
	m_pSkinnedAnimationController1->SetTrackEnable(1, false);  // Run

	m_pSkinnedAnimationController1->SetTrackPosition(0, 0.0f);
	m_pSkinnedAnimationController1->SetTrackPosition(1, 0.0f);
}

void CBoss::SetRunAnimTrack()
{
	if (m_pSkinnedAnimationController == nullptr || m_pSkinnedAnimationController1 == nullptr) return;
	// ================= 상체 =========================
	m_pSkinnedAnimationController->SetTrackEnable(0, false);
	m_pSkinnedAnimationController->SetTrackEnable(1, true); // 달리기
	m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_pSkinnedAnimationController->SetTrackEnable(3, false);

	m_pSkinnedAnimationController->SetTrackPosition(0, 0.0f);
	m_pSkinnedAnimationController->SetTrackPosition(1, 0.0f);
	m_pSkinnedAnimationController->SetTrackPosition(2, 0.0f);
	m_pSkinnedAnimationController->SetTrackPosition(3, 0.0f);

	// ===============  하체 ===========================
	m_pSkinnedAnimationController1->SetTrackEnable(0, false); // IDLE
	m_pSkinnedAnimationController1->SetTrackEnable(1, true);  // Run

	m_pSkinnedAnimationController1->SetTrackPosition(0, 0.0f); 
	m_pSkinnedAnimationController1->SetTrackPosition(1, 0.0f);
	
}

void CBoss::SetAttackAnimTrack()
{
	if (m_pSkinnedAnimationController == nullptr || m_pSkinnedAnimationController1 == nullptr) return;
	m_pSkinnedAnimationController->SetTrackEnable(0, false);
	m_pSkinnedAnimationController->SetTrackEnable(1, false);
	m_pSkinnedAnimationController->SetTrackEnable(2, true); // 공격
	m_pSkinnedAnimationController->SetTrackEnable(3, false);

	m_pSkinnedAnimationController->SetTrackPosition(0, 0.0f);
	m_pSkinnedAnimationController->SetTrackPosition(1, 0.0f);
	m_pSkinnedAnimationController->SetTrackPosition(2, 0.0f);
	m_pSkinnedAnimationController->SetTrackPosition(3, 0.0f);

	// ===============  하체 ===========================
	m_pSkinnedAnimationController1->SetTrackEnable(0, true); // IDLE
	m_pSkinnedAnimationController1->SetTrackEnable(1, false);  // Run

	m_pSkinnedAnimationController1->SetTrackPosition(0, 0.0f);
	m_pSkinnedAnimationController1->SetTrackPosition(1, 0.0f);

	//m_InteractionCountTime -= 1;
}

void CBoss::SetRunAttackAnimTrack()
{
	if (m_pSkinnedAnimationController == nullptr || m_pSkinnedAnimationController1 == nullptr) return;
	m_pSkinnedAnimationController->SetTrackEnable(0, false);
	m_pSkinnedAnimationController->SetTrackEnable(1, false);
	m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_pSkinnedAnimationController->SetTrackEnable(3, true);

	m_pSkinnedAnimationController->SetTrackPosition(0, 0.0f);
	m_pSkinnedAnimationController->SetTrackPosition(1, 0.0f);
	m_pSkinnedAnimationController->SetTrackPosition(2, 0.0f);
	m_pSkinnedAnimationController->SetTrackPosition(3, 0.0f);

	// ===============  하체 ===========================
	m_pSkinnedAnimationController1->SetTrackEnable(0, false); // IDLE
	m_pSkinnedAnimationController1->SetTrackEnable(1, true);  // Run

	m_pSkinnedAnimationController1->SetTrackPosition(0, 0.0f);
	m_pSkinnedAnimationController1->SetTrackPosition(1, 0.0f);

	//m_InteractionCountTime -= 1;
}

void CBoss::AnimTrackUpdate()
{
	switch (m_behavior)
	{
		case (int32)PLAYER_BEHAVIOR::IDLE:
			SetIdleAnimTrack();
			break;
		case(int32)PLAYER_BEHAVIOR::RUN:
			SetRunAnimTrack();
			break;
		case (int32)PLAYER_BEHAVIOR::ATTACK:
			SetAttackAnimTrack();
			break;
		case (int32)PLAYER_BEHAVIOR::RUN_ATTACK:
			SetRunAttackAnimTrack();
			break;
		break;
	}
}


void CBoss::ProcessInput(const int16& dwDirection)
{
	// 1. 공격 키를 눌렀을 경우 처리 
	if (dwDirection & KEY_SPACE)
	{
		if (m_bOnInteraction == false)
		{
			//보스 캐릭터 애니메이션 처리
			SetInteractionOn(true);
			
			// 05-06 공격 시, 사장님 공격 이벤트 전송
			C2S_ATTACK packet;
			packet.type = (uint8)C_PACKET_TYPE::CATTACK;
			packet.size = sizeof(C2S_ATTACK);
			packet.wf = mainGame.m_pScene->_curFrame;
			
			
			XMFLOAT3 bossPos = mainGame.m_pScene->_players[0]->GetPosition();
			XMFLOAT3 bossDir = mainGame.m_pScene->_players[0]->GetLook();
			float rayDist = 10.0f;
			if (PLAYERNUM > 1)
			{
				for (int i = 1; i < PLAYERNUM; ++i)
				{
					if (mainGame.m_pScene->_players[i]->m_playerBV.Intersects(XMLoadFloat3(&bossPos), XMLoadFloat3(&bossDir), rayDist))
					{
						packet.tidx = i;
						clientCore._client->DoSend(&packet);
						break;
					}
				}
			}
		}
	}

	Move(dwDirection, BOSS_VELOCITY);
}

