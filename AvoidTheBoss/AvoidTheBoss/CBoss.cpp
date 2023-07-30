#include "pch.h"
#include "CBoss.h"
#include "CEmployee.h"
#include "CBullet.h"
#include "clientIocpCore.h"

#include "GameFramework.h"

#include "SceneManager.h"
#include "InputManager.h"
#include "SoundManager.h";
#include "GameScene.h"

CBoss::CBoss(ID3D12Device5* pd3dDevice, ID3D12GraphicsCommandList4  * pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_type = 0;

	m_ctype = (uint8)PLAYER_TYPE::BOSS;
	m_nCharacterType = CHARACTER_TYPE::BOSS;
	m_pCamera = ChangeCamera(FIRST_PERSON_CAMERA, 0.0f);

		CLoadedModelInfo* pBossArmModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Character/Boss_Idle_First.bin", NULL, Layout::PLAYER);
		SetChild(pBossArmModel->m_pModelRootObject, true);
		m_pSkinnedAnimationController2 = new CAnimationController(pd3dDevice, pd3dCommandList, 4, pBossArmModel);

		m_pSkinnedAnimationController2->SetTrackAnimationSet(0, 0);//Idle
		m_pSkinnedAnimationController2->SetTrackAnimationSet(1, 1);//Run
		m_pSkinnedAnimationController2->SetTrackAnimationSet(2, 2);//Shoot
		m_pSkinnedAnimationController2->SetTrackAnimationSet(3, 3);//RunningShoot 

		CLoadedModelInfo* pBossUpperModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Character/Boss_Shooting_Run_UpperBody.bin", NULL, Layout::PLAYER);
		SetChild(pBossUpperModel->m_pModelRootObject, true);
		CLoadedModelInfo* pBossLowerModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Character/Boss_Shooting_Run_LowerBody.bin", NULL, Layout::PLAYER);
		SetChild(pBossLowerModel->m_pModelRootObject, true);
		m_pSkinnedAnimationController =  new CAnimationController(pd3dDevice, pd3dCommandList, 4, pBossUpperModel);
		m_pSkinnedAnimationController1 = new CAnimationController(pd3dDevice, pd3dCommandList, 4, pBossLowerModel);

		m_pSkinnedAnimationController->SetTrackAnimationSet(0, 1);//Idle
		m_pSkinnedAnimationController->SetTrackAnimationSet(1, 2);//Run
		m_pSkinnedAnimationController->SetTrackAnimationSet(2, 3);//Shoot 2
		m_pSkinnedAnimationController->SetTrackAnimationSet(3, 0);//RunningShoot 
		
		m_pSkinnedAnimationController1->SetTrackAnimationSet(0, 1);//Idle
		m_pSkinnedAnimationController1->SetTrackAnimationSet(1, 0);//Run
		m_pSkinnedAnimationController1->SetTrackAnimationSet(2, 2);//Run
		m_pSkinnedAnimationController1->SetTrackAnimationSet(3, 3);//Run

	if (m_pCamera->m_nMode == (DWORD)FIRST_PERSON_CAMERA)
	{
		m_pSkinnedAnimationController2->SetTrackEnable(0, true);
		m_pSkinnedAnimationController2->SetTrackEnable(1, false);
		m_pSkinnedAnimationController2->SetTrackEnable(2, false);
		m_pSkinnedAnimationController2->SetTrackEnable(3, false);

		m_pSkinnedAnimationController->SetTrackEnable(0, false);
		m_pSkinnedAnimationController->SetTrackEnable(1, false);
		m_pSkinnedAnimationController->SetTrackEnable(2, false);
		m_pSkinnedAnimationController->SetTrackEnable(3, false);

		m_pSkinnedAnimationController1->SetTrackEnable(0, false);
		m_pSkinnedAnimationController1->SetTrackEnable(1, false);
		m_pSkinnedAnimationController1->SetTrackEnable(2, false);
		m_pSkinnedAnimationController1->SetTrackEnable(3, false);
	}
	else if (m_pCamera->m_nMode == (DWORD)THIRD_PERSON_CAMERA) 
	{
		m_pSkinnedAnimationController2->SetTrackEnable(0, false);
		m_pSkinnedAnimationController2->SetTrackEnable(1, false);
		m_pSkinnedAnimationController2->SetTrackEnable(2, false);
		m_pSkinnedAnimationController2->SetTrackEnable(3, false);

		m_pSkinnedAnimationController->SetTrackEnable(0, true);
		m_pSkinnedAnimationController->SetTrackEnable(1, false);
		m_pSkinnedAnimationController->SetTrackEnable(2, false);
		m_pSkinnedAnimationController->SetTrackEnable(3, false);

		m_pSkinnedAnimationController1->SetTrackEnable(0, true);
		m_pSkinnedAnimationController1->SetTrackEnable(1, false);
		m_pSkinnedAnimationController1->SetTrackEnable(2, false);
		m_pSkinnedAnimationController1->SetTrackEnable(3, false);
	}
	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	if (pBossArmModel)delete pBossArmModel;
	if (pBossUpperModel)delete pBossUpperModel;
	if (pBossLowerModel)delete pBossLowerModel;
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
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 1.25f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(0.01f, MaxDepthofMap, ASPECT_RATIO, 60.0f); //5000.f
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
	
	Update(fTimeElapsed, m_clientType);

	return(m_pCamera);
}



void CBoss::Rotate(float x, float y, float z)
{
	CPlayer::Rotate(x, y, z);
	m_pBullet->Rotate(x, y, z);
	m_pBullet->m_pHitEffect->Rotate(x, y, z);
}

void CBoss::PrepareAnimate()
{
}

void CBoss::Move(const int16& dwDirection, float fDistance)
{	
	CPlayer::Move(dwDirection, BOSS_VELOCITY);
}

void CBoss::ResetState()
{
		SetBehavior(PLAYER_BEHAVIOR::IDLE);
	 m_runAttackAnimTime = 0;
	 m_standAttackAnimTime = 0;
	 m_IsOnAttack = false;
}

void CBoss::SetAttackAnimOtherClient()
{
	//if (!GetOnAttack())
	//{
		if (!Vector3::IsZero(m_xmf3Velocity))
		{
			SetBehavior(PLAYER_BEHAVIOR::RUN_ATTACK);
			SetRunAttackAnimTime();
		}
		else
		{
			SetBehavior(PLAYER_BEHAVIOR::ATTACK);
			SetAttackAnimTime();
		}
		SetOnAttack(true);
		m_pBullet->SetStartShoot(true);
	//}
}

void CBoss::Update(float fTimeElapsed, CLIENT_TYPE ptype)
{
	if (CLIENT_TYPE::OWNER == m_clientType)
		m_IsFirst = true;
	
	if (CLIENT_TYPE::OTHER_PLAYER  == m_clientType || m_pCamera->m_nMode == (DWORD)THIRD_PERSON_CAMERA)
		m_IsFirst = false;
	

	CPlayer::Update(fTimeElapsed, ptype);

	if (m_pBullet)
	{
		if(m_pBullet->GetStartShoot())
		{
			m_pBullet->SetDirection(GetLook());
			m_pBullet->SetStartShoot(false);
		}
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
}


void CBoss::AnimationLogicUpdate()
{
	if (GetOnAttack())
	{
		if (m_standAttackAnimTime >= BOSS_ATTACK_TIME)
		{
			SetOnAttack(false);
			SetAttackAnimTime();
			return;
		}
		m_standAttackAnimTime++;
		
	}
}

void CBoss::AimationStateUpdate()
{
	if (GetOnAttack())
	{
		if(Vector3::IsZero(m_xmf3Velocity)) SetBehavior(PLAYER_BEHAVIOR::ATTACK);
		else SetBehavior(PLAYER_BEHAVIOR::RUN_ATTACK);
	}
	else
	{
		if (Vector3::IsZero(m_xmf3Velocity)) SetBehavior(PLAYER_BEHAVIOR::IDLE);
		else SetBehavior(PLAYER_BEHAVIOR::RUN);
	}
}

void CBoss::SetIdleAnimTrack()
{
	if (CLIENT_TYPE::OWNER == m_clientType)
	{
		if (m_pSkinnedAnimationController2 == nullptr) return;
		m_pSkinnedAnimationController2->SetTrackEnable(0, true); // 아이들
		m_pSkinnedAnimationController2->SetTrackEnable(1, false);
		m_pSkinnedAnimationController2->SetTrackEnable(2, false);
		m_pSkinnedAnimationController2->SetTrackEnable(3, false);

		m_pSkinnedAnimationController2->SetTrackPosition(0, 0.0f);
		m_pSkinnedAnimationController2->SetTrackPosition(1, 0.0f);
		m_pSkinnedAnimationController2->SetTrackPosition(2, 0.0f);
		m_pSkinnedAnimationController2->SetTrackPosition(3, 0.0f);

		if (m_pSkinnedAnimationController == nullptr) return;
		m_pSkinnedAnimationController->SetTrackEnable(0, false); // 아이들
		m_pSkinnedAnimationController->SetTrackEnable(1, false);
		m_pSkinnedAnimationController->SetTrackEnable(2, false);
		m_pSkinnedAnimationController->SetTrackEnable(3, false);

		m_pSkinnedAnimationController->SetTrackPosition(0, 0.0f);
		m_pSkinnedAnimationController->SetTrackPosition(1, 0.0f);
		m_pSkinnedAnimationController->SetTrackPosition(2, 0.0f);
		m_pSkinnedAnimationController->SetTrackPosition(3, 0.0f);

		// ===============  하체 ===========================
		if (m_pSkinnedAnimationController1 == nullptr) return;
		m_pSkinnedAnimationController1->SetTrackEnable(0, false); // IDLE
		m_pSkinnedAnimationController1->SetTrackEnable(1, false);  // Run

		m_pSkinnedAnimationController1->SetTrackPosition(0, 0.0f);
		m_pSkinnedAnimationController1->SetTrackPosition(1, 0.0f);
	}
	else if (CLIENT_TYPE::OTHER_PLAYER == m_clientType)
	{
		if (m_pSkinnedAnimationController2 == nullptr) return;
		m_pSkinnedAnimationController2->SetTrackEnable(0, false); // 아이들
		m_pSkinnedAnimationController2->SetTrackEnable(1, false);
		m_pSkinnedAnimationController2->SetTrackEnable(2, false);
		m_pSkinnedAnimationController2->SetTrackEnable(3, false);

		m_pSkinnedAnimationController2->SetTrackPosition(0, 0.0f);
		m_pSkinnedAnimationController2->SetTrackPosition(1, 0.0f);
		m_pSkinnedAnimationController2->SetTrackPosition(2, 0.0f);
		m_pSkinnedAnimationController2->SetTrackPosition(3, 0.0f);

		if (m_pSkinnedAnimationController == nullptr) return;
		m_pSkinnedAnimationController->SetTrackEnable(0, true); // 아이들
		m_pSkinnedAnimationController->SetTrackEnable(1, false);
		m_pSkinnedAnimationController->SetTrackEnable(2, false);
		m_pSkinnedAnimationController->SetTrackEnable(3, false);

		m_pSkinnedAnimationController->SetTrackPosition(0, 0.0f);
		m_pSkinnedAnimationController->SetTrackPosition(1, 0.0f);
		m_pSkinnedAnimationController->SetTrackPosition(2, 0.0f);
		m_pSkinnedAnimationController->SetTrackPosition(3, 0.0f);

		// ===============  하체 ===========================
		if (m_pSkinnedAnimationController1 == nullptr) return;
		m_pSkinnedAnimationController1->SetTrackEnable(0, true); // IDLE
		m_pSkinnedAnimationController1->SetTrackEnable(1, false);  // Run

		m_pSkinnedAnimationController1->SetTrackPosition(0, 0.0f);
		m_pSkinnedAnimationController1->SetTrackPosition(1, 0.0f);
	}
}

void CBoss::SetRunAnimTrack()
{
	if (CLIENT_TYPE::OWNER == m_clientType)
	{
		if (m_pSkinnedAnimationController2 == nullptr) return;
		m_pSkinnedAnimationController2->SetTrackEnable(0, false);
		m_pSkinnedAnimationController2->SetTrackEnable(1, true);
		m_pSkinnedAnimationController2->SetTrackEnable(2, false);
		m_pSkinnedAnimationController2->SetTrackEnable(3, false);

		m_pSkinnedAnimationController2->SetTrackPosition(0, 0.0f);
		m_pSkinnedAnimationController2->SetTrackPosition(1, 0.0f);
		m_pSkinnedAnimationController2->SetTrackPosition(2, 0.0f);
		m_pSkinnedAnimationController2->SetTrackPosition(3, 0.0f);

		if (m_pSkinnedAnimationController == nullptr) return;
		// ================= 상체 =========================
		m_pSkinnedAnimationController->SetTrackEnable(0, false);
		m_pSkinnedAnimationController->SetTrackEnable(1, false); // 달리기
		m_pSkinnedAnimationController->SetTrackEnable(2, false);
		m_pSkinnedAnimationController->SetTrackEnable(3, false);

		m_pSkinnedAnimationController->SetTrackPosition(0, 0.0f);
		m_pSkinnedAnimationController->SetTrackPosition(1, 0.0f);
		m_pSkinnedAnimationController->SetTrackPosition(2, 0.0f);
		m_pSkinnedAnimationController->SetTrackPosition(3, 0.0f);

		// ===============  하체 ===========================
		if (m_pSkinnedAnimationController1 == nullptr) return;
		m_pSkinnedAnimationController1->SetTrackEnable(0, false); // IDLE
		m_pSkinnedAnimationController1->SetTrackEnable(1, false);  // Run

		m_pSkinnedAnimationController1->SetTrackPosition(0, 0.0f);
		m_pSkinnedAnimationController1->SetTrackPosition(1, 0.0f);
	}
	else if (CLIENT_TYPE::OTHER_PLAYER == m_clientType)
	{
		if (m_pSkinnedAnimationController2 == nullptr) return;
		m_pSkinnedAnimationController2->SetTrackEnable(0, false);
		m_pSkinnedAnimationController2->SetTrackEnable(1, false);
		m_pSkinnedAnimationController2->SetTrackEnable(2, false);
		m_pSkinnedAnimationController2->SetTrackEnable(3, false);

		m_pSkinnedAnimationController2->SetTrackPosition(0, 0.0f);
		m_pSkinnedAnimationController2->SetTrackPosition(1, 0.0f);
		m_pSkinnedAnimationController2->SetTrackPosition(2, 0.0f);
		m_pSkinnedAnimationController2->SetTrackPosition(3, 0.0f);

		if (m_pSkinnedAnimationController == nullptr) return;
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
		if (m_pSkinnedAnimationController1 == nullptr) return;
		m_pSkinnedAnimationController1->SetTrackEnable(0, false); // IDLE
		m_pSkinnedAnimationController1->SetTrackEnable(1, true);  // Run

		m_pSkinnedAnimationController1->SetTrackPosition(0, 0.0f);
		m_pSkinnedAnimationController1->SetTrackPosition(1, 0.0f);
	}
	
}

void CBoss::SetAttackAnimTrack()
{
	if (CLIENT_TYPE::OWNER == m_clientType)
	{
		if (m_pSkinnedAnimationController2 == nullptr) return;
		m_pSkinnedAnimationController2->SetTrackEnable(0, false); // 아이들
		m_pSkinnedAnimationController2->SetTrackEnable(1, false);
		m_pSkinnedAnimationController2->SetTrackEnable(2, true);
		m_pSkinnedAnimationController2->SetTrackEnable(3, false);

		m_pSkinnedAnimationController2->SetTrackPosition(0, 0.0f);
		m_pSkinnedAnimationController2->SetTrackPosition(1, 0.0f);
		m_pSkinnedAnimationController2->SetTrackPosition(2, 0.0f);
		m_pSkinnedAnimationController2->SetTrackPosition(3, 0.0f);

		m_pSkinnedAnimationController2->SetTrackPosition(0, 0.0f);
		m_pSkinnedAnimationController2->SetTrackPosition(1, 0.0f);
		m_pSkinnedAnimationController2->SetTrackPosition(2, 0.0f);
		m_pSkinnedAnimationController2->SetTrackPosition(3, 0.0f);

		if (m_pSkinnedAnimationController == nullptr) return;
		m_pSkinnedAnimationController->SetTrackEnable(0, false);
		m_pSkinnedAnimationController->SetTrackEnable(1, false);
		m_pSkinnedAnimationController->SetTrackEnable(2, false); // 공격
		m_pSkinnedAnimationController->SetTrackEnable(3, false);

		m_pSkinnedAnimationController->SetTrackPosition(0, 0.0f);
		m_pSkinnedAnimationController->SetTrackPosition(1, 0.0f);
		m_pSkinnedAnimationController->SetTrackPosition(2, 0.0f);
		m_pSkinnedAnimationController->SetTrackPosition(3, 0.0f);

		// ===============  하체 ===========================
		if (m_pSkinnedAnimationController1 == nullptr) return;
		m_pSkinnedAnimationController1->SetTrackEnable(0, false); // IDLE
		m_pSkinnedAnimationController1->SetTrackEnable(1, false);  // Run

		m_pSkinnedAnimationController1->SetTrackPosition(0, 0.0f);
		m_pSkinnedAnimationController1->SetTrackPosition(1, 0.0f);

	}
	else if (CLIENT_TYPE::OTHER_PLAYER == m_clientType)
	{
		if (m_pSkinnedAnimationController2 == nullptr) return;
		m_pSkinnedAnimationController2->SetTrackEnable(0, false); // 아이들
		m_pSkinnedAnimationController2->SetTrackEnable(1, false);
		m_pSkinnedAnimationController2->SetTrackEnable(2, false);
		m_pSkinnedAnimationController2->SetTrackEnable(3, false);

		m_pSkinnedAnimationController2->SetTrackPosition(0, 0.0f);
		m_pSkinnedAnimationController2->SetTrackPosition(1, 0.0f);
		m_pSkinnedAnimationController2->SetTrackPosition(2, 0.0f);
		m_pSkinnedAnimationController2->SetTrackPosition(3, 0.0f);

		if (m_pSkinnedAnimationController == nullptr) return;
		m_pSkinnedAnimationController->SetTrackEnable(0, false);
		m_pSkinnedAnimationController->SetTrackEnable(1, false);
		m_pSkinnedAnimationController->SetTrackEnable(2, true); // 공격
		m_pSkinnedAnimationController->SetTrackEnable(3, false);

		m_pSkinnedAnimationController->SetTrackPosition(0, 0.0f);
		m_pSkinnedAnimationController->SetTrackPosition(1, 0.0f);
		m_pSkinnedAnimationController->SetTrackPosition(2, 0.0f);
		m_pSkinnedAnimationController->SetTrackPosition(3, 0.0f);

		// ===============  하체 ===========================
		if (m_pSkinnedAnimationController1 == nullptr) return;
		m_pSkinnedAnimationController1->SetTrackEnable(0, true); // IDLE
		m_pSkinnedAnimationController1->SetTrackEnable(1, false);  // Run

		m_pSkinnedAnimationController1->SetTrackPosition(0, 0.0f);
		m_pSkinnedAnimationController1->SetTrackPosition(1, 0.0f);
	}
}

void CBoss::SetRunAttackAnimTrack()
{
	if (CLIENT_TYPE::OWNER == m_clientType)
	{
		if (m_pSkinnedAnimationController2 == nullptr) return;
		m_pSkinnedAnimationController2->SetTrackEnable(0, false);
		m_pSkinnedAnimationController2->SetTrackEnable(1, false);
		m_pSkinnedAnimationController2->SetTrackEnable(2, false);
		m_pSkinnedAnimationController2->SetTrackEnable(3, true);

		m_pSkinnedAnimationController2->SetTrackPosition(0, 0.0f);
		m_pSkinnedAnimationController2->SetTrackPosition(1, 0.0f);
		m_pSkinnedAnimationController2->SetTrackPosition(2, 0.0f);
		m_pSkinnedAnimationController2->SetTrackPosition(3, 0.0f);

		if (m_pSkinnedAnimationController == nullptr) return;
		m_pSkinnedAnimationController->SetTrackEnable(0, false);
		m_pSkinnedAnimationController->SetTrackEnable(1, false);
		m_pSkinnedAnimationController->SetTrackEnable(2, false);
		m_pSkinnedAnimationController->SetTrackEnable(3, false);

		m_pSkinnedAnimationController->SetTrackPosition(0, 0.0f);
		m_pSkinnedAnimationController->SetTrackPosition(1, 0.0f);
		m_pSkinnedAnimationController->SetTrackPosition(2, 0.0f);
		m_pSkinnedAnimationController->SetTrackPosition(3, 0.0f);

		if (m_pSkinnedAnimationController1 == nullptr) return;
		// ===============  하체 ===========================
		m_pSkinnedAnimationController1->SetTrackEnable(0, false); // IDLE
		m_pSkinnedAnimationController1->SetTrackEnable(1, false);  // Run

		m_pSkinnedAnimationController1->SetTrackPosition(0, 0.0f);
		m_pSkinnedAnimationController1->SetTrackPosition(1, 0.0f);
	}
	else if (CLIENT_TYPE::OTHER_PLAYER == m_clientType)
	{
		if (m_pSkinnedAnimationController2 == nullptr) return;
		m_pSkinnedAnimationController2->SetTrackEnable(0, false);
		m_pSkinnedAnimationController2->SetTrackEnable(1, false);
		m_pSkinnedAnimationController2->SetTrackEnable(2, false);
		m_pSkinnedAnimationController2->SetTrackEnable(3, false);

		m_pSkinnedAnimationController2->SetTrackPosition(0, 0.0f);
		m_pSkinnedAnimationController2->SetTrackPosition(1, 0.0f);
		m_pSkinnedAnimationController2->SetTrackPosition(2, 0.0f);
		m_pSkinnedAnimationController2->SetTrackPosition(3, 0.0f);

		if (m_pSkinnedAnimationController == nullptr) return;
		m_pSkinnedAnimationController->SetTrackEnable(0, false);
		m_pSkinnedAnimationController->SetTrackEnable(1, false);
		m_pSkinnedAnimationController->SetTrackEnable(2, false);
		m_pSkinnedAnimationController->SetTrackEnable(3, true);

		m_pSkinnedAnimationController->SetTrackPosition(0, 0.0f);
		m_pSkinnedAnimationController->SetTrackPosition(1, 0.0f);
		m_pSkinnedAnimationController->SetTrackPosition(2, 0.0f);
		m_pSkinnedAnimationController->SetTrackPosition(3, 0.0f);

		if (m_pSkinnedAnimationController1 == nullptr) return;
		// ===============  하체 ===========================
		m_pSkinnedAnimationController1->SetTrackEnable(0, false); // IDLE
		m_pSkinnedAnimationController1->SetTrackEnable(1, true);  // Run

		m_pSkinnedAnimationController1->SetTrackPosition(0, 0.0f);
		m_pSkinnedAnimationController1->SetTrackPosition(1, 0.0f);
	}
}

void CBoss::AnimTrackUpdate()
{
	switch (m_behavior)
	{
		case (int32)PLAYER_BEHAVIOR::IDLE:
			SetIdleAnimTrack();
			if (GetOnMoveSound())
			{
				SetOnMoveSound(false);
				SoundManager::SoundStop(4);
			}
			break;
		case(int32)PLAYER_BEHAVIOR::RUN:
			SetRunAnimTrack();
			if (!GetOnMoveSound())
			{
				SoundManager::GetInstance().PlayObjectSound(10, 4);
				SetOnMoveSound(true);
			}
			break;
		case (int32)PLAYER_BEHAVIOR::ATTACK:
			SetAttackAnimTrack();
			break;
		case (int32)PLAYER_BEHAVIOR::RUN_ATTACK:
			break;
	}
}


uint8 CBoss::ProcessInput()
{
	uint8 dir = 0;

	if (InputManager::GetInstance().GetKeyBuffer(KEY_TYPE::W) > 0)  dir |= KEY_FORWARD;
	if (InputManager::GetInstance().GetKeyBuffer(KEY_TYPE::A) > 0)  dir |= KEY_LEFT;
	if (InputManager::GetInstance().GetKeyBuffer(KEY_TYPE::S) > 0)  dir |= KEY_BACKWARD;
	if (InputManager::GetInstance().GetKeyBuffer(KEY_TYPE::D) > 0)  dir |= KEY_RIGHT;

	if (dir) SetBehavior(PLAYER_BEHAVIOR::RUN);
	else	 SetBehavior(PLAYER_BEHAVIOR::IDLE);

	CGameScene* gs = static_cast<CGameScene*>(mainGame.m_SceneManager->GetSceneByIdx(3));
	// 1. 공격 키를 눌렀을 경우 처리 
	if (InputManager::GetInstance().GetKeyBuffer(KEY_TYPE::SPACE) == (uint8)KEY_STATUS::KEY_PRESS && !GetOnAttack())
	{
		SetOnAttack(true);
		SoundManager::GetInstance().PlayObjectSound(2, 3);

		C2S_ATTACK packet;
		packet.type = (uint8)C_GAME_PACKET_TYPE::CATTACK;
		packet.size = sizeof(C2S_ATTACK);
		packet.wf = mainGame.m_curFrame;

		SC_EVENTPACKET epacket;
		epacket.size = sizeof(packet);
		epacket.type = (uint8)SC_GAME_PACKET_TYPE::GAMEEVENT;
		epacket.eventId = (uint8)EVENT_TYPE::ATTACK_ANIM;

		clientCore.DoSend(&epacket);

		XMFLOAT3 bossPos = GetPosition();
		XMFLOAT3 bossDir = GetLook();
		float rayDist = 10.0f;

		CGameScene* gs = static_cast<CGameScene*>(mainGame.m_SceneManager->GetSceneByIdx((int32)CGameFramework::SCENESTATE::INGAME));

		if (PLAYERNUM >= 1)
		{
			for (int i = 1; i < PLAYERNUM; ++i)
			{
				CEmployee* targetPlayer = static_cast<CEmployee*>(gs->GetScenePlayerByIdx(i));
				if (targetPlayer)
				{
					if (targetPlayer->m_playerBV.Intersects(XMLoadFloat3(&bossPos), XMLoadFloat3(&bossDir), rayDist) && !targetPlayer->m_bIsInvincibility)
					{
						packet.tidx = i;
						DelayEvent* aev = new DelayEvent(packet);
						gs->AddEvent(static_cast<queueEvent*>(aev), 0);
						break;
					}
				}
			}
		}
		m_pBullet->SetOnShoot(true);
		m_pBullet->SetStartShoot(true);
		SoundManager::GetInstance().PlayObjectSound(4, 3);
	}
	if (InputManager::GetInstance().GetKeyBuffer(KEY_TYPE::G) == (uint8)KEY_STATUS::KEY_PRESS)
	{
		if (m_pCamera->m_fogOn) m_pCamera->m_fogOn = false;
		else m_pCamera->m_fogOn = true;
	}

	Move(dir, BOSS_VELOCITY);
	return dir;
}

