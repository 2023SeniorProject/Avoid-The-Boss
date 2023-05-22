#include "pch.h"
#include "CBoss.h"
#include "CBullet.h"
#include "clientIocpCore.h"


CBoss::CBoss(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_type = 0;
	m_pCamera = ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);

	m_nCharacterType = CHARACTER_TYPE::BOSS;

	//CLoadedModelInfo* pBossModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, g_pstrCharactorRefernece[(int)m_nCharacterType], NULL, Layout::PLAYER);
	//SetChild(pBossModel->m_pModelRootObject, true);

	CLoadedModelInfo* pBossUpperModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Boss_Shooting_Run_UpperBody(3).bin", NULL, Layout::PLAYER);
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
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 1.7f * UNIT, -5 * UNIT));
		m_pCamera->GenerateProjectionMatrix(1.01f, MaxDepthofMap, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	default:
		break;
	}
	m_pCamera->SetPosition(Vector3::Add(m_xmf3Position, m_pCamera->GetOffset()));

	Update(fTimeElapsed, PLAYER_TYPE::NONE);

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
	if (m_pSkinnedAnimationController && m_pSkinnedAnimationController1)
	{
		if (LOBYTE(dwDirection)) // 1. ĳ���� �̵����� ���� �� (WASD Ű �Է�)
		{
			if (!m_OnInteraction) // ���� Ű ���Է�, �̵�Ű �Է� --> �޸���
			{
				// ��ü
				m_pSkinnedAnimationController->SetTrackEnable(0, false);
				m_pSkinnedAnimationController->SetTrackEnable(1, true);
				m_pSkinnedAnimationController->SetTrackEnable(2, false);
				m_pSkinnedAnimationController->SetTrackEnable(3, false);

				m_pSkinnedAnimationController->SetTrackPosition(0, 0.0f);
				m_pSkinnedAnimationController->SetTrackPosition(1, 0.0f);
				m_pSkinnedAnimationController->SetTrackPosition(2, 0.0f);
				m_pSkinnedAnimationController->SetTrackPosition(3, 0.0f);

				
			}
			else // ���� Ű, �̵�Ű ��� �Է� --> �޸��鼭 ���
			{
				if (m_InteractionCountTime == BOSS_INTERACTION_TIME)
				{
					// ��ü
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
				else if (m_InteractionCountTime < BOSS_INTERACTION_TIME)
				{
					if(m_InteractionCountTime <=0)
						m_OnInteraction = false;

					m_InteractionCountTime -= 1;
				}
			}

			// ��ü
			m_pSkinnedAnimationController1->SetTrackEnable(0, false);
			m_pSkinnedAnimationController1->SetTrackEnable(1, true);

			m_pSkinnedAnimationController1->SetTrackPosition(0, 0.0f);
			m_pSkinnedAnimationController1->SetTrackPosition(1, 0.0f);
		}
		else if (LOBYTE(dwDirection) == 0) // �̵�Ű �Է��� �ƴ� ��
		{
			if (!m_OnInteraction) // ���� Ű, �̵�Ű ��� ���Է�
			{
				m_pSkinnedAnimationController->SetTrackEnable(0, true);
				m_pSkinnedAnimationController->SetTrackEnable(1, false);
				m_pSkinnedAnimationController->SetTrackEnable(2, false);
				m_pSkinnedAnimationController->SetTrackEnable(3, false);

				m_pSkinnedAnimationController->SetTrackPosition(0, 0.0f);
				m_pSkinnedAnimationController->SetTrackPosition(1, 0.0f);
				m_pSkinnedAnimationController->SetTrackPosition(2, 0.0f);
				m_pSkinnedAnimationController->SetTrackPosition(3, 0.0f);
			}
			else // ���� Ű�� �Է�, �̵�Ű�� ���Է�
			{
				if (m_InteractionCountTime == BOSS_INTERACTION_TIME)
				{
					m_pSkinnedAnimationController->SetTrackEnable(0, false);
					m_pSkinnedAnimationController->SetTrackEnable(1, false);
					m_pSkinnedAnimationController->SetTrackEnable(2, true);
					m_pSkinnedAnimationController->SetTrackEnable(3, false);

					m_pSkinnedAnimationController->SetTrackPosition(0, 0.0f);
					m_pSkinnedAnimationController->SetTrackPosition(1, 0.0f);
					m_pSkinnedAnimationController->SetTrackPosition(2, 0.0f);
					m_pSkinnedAnimationController->SetTrackPosition(3, 0.0f);

					m_InteractionCountTime -= 1;
				}
				else if (m_InteractionCountTime < BOSS_INTERACTION_TIME)
				{
					if (m_InteractionCountTime <= 0)
						m_OnInteraction = false;

					m_InteractionCountTime -= 1;
				}
			}

			// ��ü
			m_pSkinnedAnimationController1->SetTrackEnable(0, true);
			m_pSkinnedAnimationController1->SetTrackEnable(1, false);

			m_pSkinnedAnimationController1->SetTrackPosition(0, 0.0f);
			m_pSkinnedAnimationController1->SetTrackPosition(1, 0.0f);
		}
	}

	CPlayer::Move(dwDirection, fDistance);
   
}

void CBoss::Update(float fTimeElapsed, PLAYER_TYPE ptype)
{
	CPlayer::Update(fTimeElapsed, ptype);

	
	if (m_pBullet)
	{
		m_pBullet->SetBulletPosition(GetPosition());
		m_pBullet->Update(fTimeElapsed);
	}
	if (ptype == PLAYER_TYPE::OWNER) m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	else if(m_pBullet) AttackAnimationOn();
	
}


void CBoss::ProcessInput(const int16& dwDirection)
{
	// 1. Set Move Speed
	Move(dwDirection, BOSS_VELOCITY);

	// 2. ���� Ű�� ������ ��� ó�� 
	if (dwDirection & KEY_SPACE)
	{
		if (m_InteractionCountTime <= 0 && m_OnInteraction == false)
		{
			SetInteractionAnimation(true);
			m_InteractionCountTime = BOSS_INTERACTION_TIME;
			// 05-06 ���� ��, ����� ���� �̺�Ʈ ����
			SC_EVENTPACKET packet;
			packet.eventId = (uint8)EVENT_TYPE::ATTACK_EVENT;
			packet.type = SC_PACKET_TYPE::GAMEEVENT;
			packet.size = sizeof(SC_EVENTPACKET);
			clientCore._client->DoSend(&packet);
		}
	}
}

