#include "pch.h"
#include "CEmployee.h"
#include "clientIocpCore.h"
#include "GameFramework.h"
#include "InputManager.h"
#include "Player.h"


CEmployee::CEmployee(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CHARACTER_TYPE nType)
{
	m_pCamera = ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);

	m_nCharacterType = nType;

	m_InteractionCountTime = 40;

	CLoadedModelInfo* pEmployeeModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, g_pstrCharactorRefernece[(int)m_nCharacterType], NULL, Layout::PLAYER);
	SetChild(pEmployeeModel->m_pModelRootObject, true);

	m_pSkinnedAnimationController = new CAnimationController(pd3dDevice, pd3dCommandList, 7, pEmployeeModel);
	m_pSkinnedAnimationController->SetTrackAnimationSet(2, 0);//faint_down
	m_pSkinnedAnimationController->SetTrackAnimationSet(3, 1);//down_idle
	m_pSkinnedAnimationController->SetTrackAnimationSet(0, 2);//idle
	m_pSkinnedAnimationController->SetTrackAnimationSet(1, 3);//run
	m_pSkinnedAnimationController->SetTrackAnimationSet(4, 4);//walk
	m_pSkinnedAnimationController->SetTrackAnimationSet(5, 5);//awake
	m_pSkinnedAnimationController->SetTrackAnimationSet(6, 6);//button

	m_pSkinnedAnimationController->SetTrackEnable(0, true);
	m_pSkinnedAnimationController->SetTrackEnable(1, false);
	m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_pSkinnedAnimationController->SetTrackEnable(3, false);
	m_pSkinnedAnimationController->SetTrackEnable(4, false);
	m_pSkinnedAnimationController->SetTrackEnable(5, false);
	m_pSkinnedAnimationController->SetTrackEnable(6, false);
	//m_pSkinnedAnimationController->SetTrackEnable(7, false);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	//SetPlayerUpdatedContext();
	//SetCameraUpdatedContext();

	//SetScale(XMFLOAT3(1.f, 1.f, 1.f));
	SetPosition(XMFLOAT3(0.0f, 0.25f, -30.0f));//-30.0f

	if (pEmployeeModel) delete pEmployeeModel;
}

CEmployee::~CEmployee()
{
}

CCamera* CEmployee::ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed)
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
		m_pCamera->SetPosition(XMFLOAT3(0, 2.25, 0));
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

	Update(fTimeElapsed, PLAYER_TYPE::OWNER);

	return(m_pCamera);
}


void CEmployee::Move(DWORD dwDirection, float fDistance)
{
	
	if (LOBYTE(dwDirection) && !m_OnInteraction) // �̵� �߿� 
	{
		m_pSkinnedAnimationController->SetTrackEnable(0, false); // �ȱ�
		m_pSkinnedAnimationController->SetTrackEnable(1, true); // �޸���
		m_pSkinnedAnimationController->SetTrackEnable(6, false);
		m_pSkinnedAnimationController->SetTrackPosition(0, 0);
		m_pSkinnedAnimationController->SetTrackPosition(6, 0);
	}
	else if (!LOBYTE(dwDirection) && !m_OnInteraction)
	{
		m_pSkinnedAnimationController->SetTrackEnable(0, true); // ���̵� ����
		m_pSkinnedAnimationController->SetTrackEnable(1, false); // �޸���
		m_pSkinnedAnimationController->SetTrackEnable(6, false);
		m_pSkinnedAnimationController->SetTrackPosition(1, 0);
		m_pSkinnedAnimationController->SetTrackPosition(6, 0);
	}
	else if (m_OnInteraction)
	{
		m_pSkinnedAnimationController->SetTrackEnable(0, false);
		m_pSkinnedAnimationController->SetTrackEnable(1, false);
		m_pSkinnedAnimationController->SetTrackEnable(6, true);
		m_pSkinnedAnimationController->SetTrackPosition(0, 0);
		m_pSkinnedAnimationController->SetTrackPosition(1, 0);
	}
	CPlayer::Move(dwDirection, PLAYER_VELOCITY);
}

void CEmployee::Update(float fTimeElapsed, PLAYER_TYPE ptype)
{
	CPlayer::Update(fTimeElapsed, ptype);
	if (GetAvailableSwitchIdx() != -1) m_bIsInSwitchArea = true;
	else m_bIsInSwitchArea = false;
	
	if (ptype == PLAYER_TYPE::OWNER) m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	else if (ptype == PLAYER_TYPE::OTHER_PLAYER)
	{
		if(!Vector3::IsZero(m_xmf3Velocity)) SetInteractionAnimation(false);
	}
}
void CEmployee::OnInteractionAnimation()
{

}

void CEmployee::SwitchAnimationForOtherClient()
{
	if (m_OnInteraction)
	{
		m_pSkinnedAnimationController->SetTrackEnable(0, false);
		m_pSkinnedAnimationController->SetTrackEnable(1, false);
		m_pSkinnedAnimationController->SetTrackEnable(6, true);
		m_pSkinnedAnimationController->SetTrackPosition(0, 0);
		m_pSkinnedAnimationController->SetTrackPosition(1, 0);
	}
}

int32 CEmployee::GetAvailableSwitchIdx()
{
	for (int i = 0; i < 3; ++i)
	{
		XMFLOAT3 distanceVec = Vector3::Subtract(m_xmf3Position, m_pSwitches[i].position);
		float distance = Vector3::Length(distanceVec);
		float sumRange = m_playerBV.Radius + m_pSwitches[i].radius;
		if (distance <= sumRange)
		{
			return i;
		}
	}
	
	return -1;
}


// 04-29 ���� Ű�Է� ó�� �߰�
void CEmployee::ProcessInput(const int16& inputKey)
{
	Move(inputKey,PLAYER_VELOCITY);
	if (HIBYTE(inputKey) != 0) // ��ȣ�ۿ� Ű�� �������� ���
	{
		int32 switchIdx = GetAvailableSwitchIdx(); // �� �� ����ġ ������ �ִ��� �ľ��Ѵ�.

		if (switchIdx != -1)
		{
			CGenerator* targetGenerator = mainGame.m_pScene->m_ppSwitches[switchIdx];
			targetGenerator->m_lock.lock(); // �ٸ� �÷��̾ Ȱ��ȭ ���� ��츦 �����ؼ�
			if (IsPlayerCanSwitchInteraction() && !targetGenerator->m_bOtherPlayerInteractionOn)
				// �ٸ� �÷��̾ ��ȣ�ۿ� ���°� �ƴ� �� && �÷��̾ ����ġ ��ġ�� �ִٸ�
			{
				if (!m_bIsPlayerOnSwitchInteration && !targetGenerator->m_bSwitchActive) // �÷��̾ ��ȣ�ۿ� ���°� �ƴϿ��ٸ�
				{
					SetInteractionAnimation(true); // ĳ���� �ִϸ��̼� ����� �����Ѵ�.
					m_bIsPlayerOnSwitchInteration = true; // �÷��̾ ��ȣ�ۿ� �������� ����Ѵ�.
					targetGenerator->InteractAnimation(true); // ������ �ִϸ��̼� ����� �����Ѵ�.
					targetGenerator->SetAnimationCount(BUTTON_ANIM_FRAME);

					SC_EVENTPACKET packet;
					packet.eventId = switchIdx + 2;
					packet.size = sizeof(SC_EVENTPACKET);
					packet.type = SC_PACKET_TYPE::GAMEEVENT;
					clientCore._client->DoSend(&packet);
				}
			}
		}
	}
	
}