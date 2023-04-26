#include "pch.h"
#include "CBoss.h"
#include "CBullet.h"

CBoss::CBoss(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_type = 0;
	m_pCamera = ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);

	m_nCharacterType = CHARACTER_TYPE::BOSS;
	m_InteractionCountTime = 65;

	CLoadedModelInfo* pBossModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, g_pstrCharactorRefernece[(int)m_nCharacterType], NULL, Layout::PLAYER);
	SetChild(pBossModel->m_pModelRootObject, true);

	m_pSkinnedAnimationController = new CAnimationController(pd3dDevice, pd3dCommandList, 4, pBossModel);
	m_pSkinnedAnimationController->SetTrackAnimationSet(2, 0);//Idle
	m_pSkinnedAnimationController->SetTrackAnimationSet(0, 1);//Shoot
	m_pSkinnedAnimationController->SetTrackAnimationSet(3, 2);//RunningShoot
	m_pSkinnedAnimationController->SetTrackAnimationSet(1, 3);//Run

	m_pSkinnedAnimationController->SetTrackEnable(0, true);
	m_pSkinnedAnimationController->SetTrackEnable(1, false);
	m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_pSkinnedAnimationController->SetTrackEnable(3, false);

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
	SetPosition(XMFLOAT3(0.0f, 0.25f, -50.0f));

	m_pBullet = new CBullet(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	m_pBullet->SetPosition(XMFLOAT3(0.0f, 1.f, 0.0f));

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
	return(m_pCamera);
}

void CBoss::OnPlayerUpdateCallback()
{

}

void CBoss::OnCameraUpdateCallback()
{
}

void CBoss::Move(DWORD dwDirection, float fDistance)
{
	if (dwDirection && !m_OnInteraction)
	{
		m_pSkinnedAnimationController->SetTrackEnable(0, false);
		m_pSkinnedAnimationController->SetTrackEnable(1, true);
	}
	CPlayer::Move(dwDirection, fDistance);
}

void CBoss::Update(float fTimeElapsed, PLAYER_TYPE ptype)
{
	CPlayer::Update(fTimeElapsed, PLAYER_TYPE::OWNER);

	if (m_OnInteraction)
	{
		OnInteractive();
		m_pBullet->SetOnShoot(true);
	}

	if (m_pBullet)
		m_pBullet->Update(fTimeElapsed);

	if (m_pSkinnedAnimationController && !m_OnInteraction)
	{
		float fLength = sqrtf(m_xmf3Velocity.x * m_xmf3Velocity.x + m_xmf3Velocity.z * m_xmf3Velocity.z);
		if (::IsZero(fLength))
		{
			m_pSkinnedAnimationController->SetTrackEnable(0, true);
			m_pSkinnedAnimationController->SetTrackEnable(1, false);
			m_pSkinnedAnimationController->SetTrackPosition(0, 0.0f);
		}
	}
	if (ptype == PLAYER_TYPE::OWNER) m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

void CBoss::OnInteractive()
{
	if (m_OnInteraction == true && m_InteractionCountTime > 0)
	{
		if (m_pSkinnedAnimationController->GetTrackEnable(2)) // idle
		{
			m_pSkinnedAnimationController->SetTrackEnable(2, false);
			m_pSkinnedAnimationController->SetTrackEnable(0, true);//idle_shoot
			m_pSkinnedAnimationController->SetTrackPosition(0, 0.0f);
		}
		else if (m_pSkinnedAnimationController->GetTrackEnable(1))//run
		{
			m_pSkinnedAnimationController->SetTrackEnable(1, false);
			m_pSkinnedAnimationController->SetTrackEnable(3, true);//running shoot
			m_pSkinnedAnimationController->SetTrackPosition(0, 0.0f);
		}

		m_InteractionCountTime -= 1;
	}
	else
	{
		m_pSkinnedAnimationController->SetTrackEnable(0, false);
		m_pSkinnedAnimationController->SetTrackEnable(3, false);
		m_OnInteraction = false;
		m_InteractionCountTime = 65;
	}
}