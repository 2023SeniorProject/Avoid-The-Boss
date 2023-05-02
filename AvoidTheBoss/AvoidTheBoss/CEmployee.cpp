#include "pch.h"
#include "CEmployee.h"
#include "clientIocpCore.h"
#include "GameFramework.h"
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
	//m_pSkinnedAnimationController->SetTrackAnimationSet(7, 7);//button


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

void CEmployee::OnPlayerUpdateCallback()
{
}

void CEmployee::OnCameraUpdateCallback()
{
}

void CEmployee::Move(DWORD dwDirection, float fDistance)
{
	
	if (LOBYTE(dwDirection) && !m_OnInteraction)
	{
		m_pSkinnedAnimationController->SetTrackEnable(0, false); // 걷기
		m_pSkinnedAnimationController->SetTrackEnable(1, true); // 달리기
	}
	else if(!LOBYTE(dwDirection))
	{
		std::cout << "idle\n";
		m_pSkinnedAnimationController->SetTrackEnable(1, false); // 달리기
		m_pSkinnedAnimationController->SetTrackEnable(0, true); // 아이들 상태
		m_pSkinnedAnimationController->SetTrackPosition(0, 0.0f);
	}
	CPlayer::Move(dwDirection, fDistance);
}

void CEmployee::Update(float fTimeElapsed, PLAYER_TYPE ptype)
{
	CPlayer::Update(fTimeElapsed, ptype);

	if (GetAvailableSwitchIdx() != -1) m_bIsInSwitchArea = true;
	else m_bIsInSwitchArea = false;
	if (m_OnInteraction) OnInteractionAnimation();
	if (ptype == PLAYER_TYPE::OWNER) m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
}
void CEmployee::OnInteractionAnimation()
{
	if (m_OnInteraction == true && m_InteractionCountTime > 0)
	{
		m_pSkinnedAnimationController->SetTrackEnable(0, false);
		m_pSkinnedAnimationController->SetTrackEnable(1, false);
		m_pSkinnedAnimationController->SetTrackEnable(6, true);
		m_pSkinnedAnimationController->SetTrackPosition(0, 0.0f);

		m_InteractionCountTime -= 1;
	}
	else if(m_OnInteraction == false) // 상호작용이 멈추면 재생 취소
	{
		m_pSkinnedAnimationController->SetTrackEnable(0, true);
		m_pSkinnedAnimationController->SetTrackEnable(1, false);
		m_pSkinnedAnimationController->SetTrackEnable(6, false);
		m_OnInteraction = false;
		
	}
	else if (m_InteractionCountTime <= 0) // 상호작용 동작 연속 재생하게 한다
	{
		m_InteractionCountTime = 45;
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


// 04-29 직원 키입력 처리 추가
void CEmployee::ProcessInput(DWORD& dwDirection)
{

	static UCHAR pKeyBuffer[256];
	// 방향키를 바이트로 처리한다.
	if (::GetKeyboardState(pKeyBuffer))
	{
		if ((pKeyBuffer[0x57] & 0xF0) || (pKeyBuffer[0x77] & 0xF0)) dwDirection |= DIR_FORWARD;
		if ((pKeyBuffer[0x53] & 0xF0) || (pKeyBuffer[0x73] & 0xF0)) dwDirection |= DIR_BACKWARD;
		if ((pKeyBuffer[0x61] & 0xF0) || (pKeyBuffer[0x41] & 0xF0)) dwDirection |= DIR_LEFT;
		if ((pKeyBuffer[0x44] & 0xF0) || (pKeyBuffer[0x64] & 0xF0)) dwDirection |= DIR_RIGHT;

		int32 switchIdx = GetAvailableSwitchIdx(); // 몇 번 스위치 영역에 있는지 파악한다.
		if ((pKeyBuffer[0x46] & 0xF0) || (pKeyBuffer[0x66] & 0xF0)) // F키가 눌렸을 경우
		{

			if (switchIdx != -1)
			{
				CGenerator* targetGenerator = mainGame.m_pScene->m_ppSwitches[switchIdx];
				targetGenerator->m_lock.lock(); // 다른 플레이어가 활성화 했을 경우를 생각해서
				if (IsPlayerCanSwitchInteraction() && !targetGenerator->m_bOtherPlayerInteractionOn)
					// 다른 플레이어가 상호작용 상태가 아닐 때 && 플레이어가 스위치 위치에 있다면
				{
					if (!m_bIsPlayerOnSwitchInteration && !targetGenerator->m_bSwitchActive) // 플레이어가 상호작용 상태가 아니였다면 
					{	
						SetInteractionAnimation(true);
						m_bIsPlayerOnSwitchInteration = true;
						targetGenerator->InteractAnimation(true); // 애니메이션 재생을 시작한다.
						targetGenerator->SetAnimationCount(BUTTON_ANIM_FRAME);
						
						SC_EVENTPACKET packet;
						packet.eventId = switchIdx + 2;
						packet.size = sizeof(SC_EVENTPACKET);
						packet.type = SC_PACKET_TYPE::GAMEEVENT;
						clientCore._client->DoSend(&packet);
					}
				}
				targetGenerator->m_lock.unlock();
			}
			dwDirection = 0;
			dwDirection |= DIR_BUTTON_F;
		}
		else // F키가 안눌린 상태일 때
		{
			if (switchIdx != -1 && IsPlayerCanSwitchInteraction()) // 발전기 주변 영역에 위치할 경우
			{
				CGenerator* targetGenerator = mainGame.m_pScene->m_ppSwitches[switchIdx];
				targetGenerator->m_lock.lock();
				// 그냥 평상시 상태일 때 F키가 안눌렀을 때와 F키가 눌러져있었는데 땐 상황인지 구별
				if (m_bIsPlayerOnSwitchInteration && !targetGenerator->m_bSwitchActive) // 발전기가 다 활성화가 안되었는데 키를 땐 상황이라면
				{
				
					// 스위치 도중 캔슬 이벤트 패킷을 보낸다 --> 발전기 게이지 초기화
					targetGenerator->InteractAnimation(false); // 애니메이션 재생을 정지한다.
					SC_EVENTPACKET packet;
					packet.eventId = switchIdx + 5;
					packet.size = sizeof(SC_EVENTPACKET);
					packet.type = SC_PACKET_TYPE::GAMEEVENT;
					m_bIsPlayerOnSwitchInteration = false;
					clientCore._client->DoSend(&packet);
				}
				targetGenerator->m_lock.unlock();
			}
		}
	}
}