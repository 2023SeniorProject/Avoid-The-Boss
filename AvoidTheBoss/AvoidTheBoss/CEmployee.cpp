#include "pch.h"
#include "CEmployee.h"
#include "clientIocpCore.h"
#include "GameFramework.h"
#include "InputManager.h"

#include "Player.h"


CEmployee::CEmployee(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CHARACTER_TYPE nType)
{
	m_ctype = (uint8)PLAYER_TYPE::EMPLOYEE;
	m_nCharacterType = nType;

	m_pCamera = ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);
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
	SetPosition(XMFLOAT3(0.0f, 0.25f, -30.0f));
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

	Update(fTimeElapsed, CLIENT_TYPE::OWNER);

	return(m_pCamera);
}


void CEmployee::Move(const int16& dwDirection, float fDistance)
{
	// 플레이어의 행동을 저장.
	if (LOBYTE(dwDirection)) m_behavior = RUN;
	else if (!LOBYTE(dwDirection)) m_behavior = IDLE;
	else if (m_OnInteraction)
	{
		m_behavior = SWITCH_INTER;
		CPlayer::Move(0, PLAYER_VELOCITY);
	}
	// 플레이어 속도 셋팅
	CPlayer::Move(dwDirection, PLAYER_VELOCITY);
}
	
	


void CEmployee::Update(float fTimeElapsed, CLIENT_TYPE ptype)
{
	CPlayer::Update(fTimeElapsed, ptype);
	LateUpdate(fTimeElapsed,ptype);
}

void CEmployee::LateUpdate(float fTimeElapsed, CLIENT_TYPE ptype)
{
	// ===== 애니메이션 트랙 업데이트 ========
	AnimTrackUpdate();
	
	//======= 스위치 위치 판별 =========
	if (GetAvailableSwitchIdx() != -1) m_bIsInSwitchArea = true;
	else m_bIsInSwitchArea = false;
	
	if (ptype == CLIENT_TYPE::OWNER) m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	else if (ptype == CLIENT_TYPE::OTHER_PLAYER)
	{
		if (m_behavior != SWITCH_INTER) SetInteractionAnimation(false);
	}
}

// ===============애니메이션 트랙 ==================
void CEmployee::SetInteractionAnimTrack()
{
	if (m_pSkinnedAnimationController == nullptr) return;
	m_pSkinnedAnimationController->SetTrackEnable(0, false);
	m_pSkinnedAnimationController->SetTrackEnable(1, false);
	m_pSkinnedAnimationController->SetTrackEnable(6, true);
	m_pSkinnedAnimationController->SetTrackPosition(0, 0);
	m_pSkinnedAnimationController->SetTrackPosition(1, 0);
	m_pSkinnedAnimationController->SetTrackPosition(6, 0);
}

void CEmployee::SetIdleAnimTrack()
{
	if (m_pSkinnedAnimationController == nullptr) return;
	m_pSkinnedAnimationController->SetTrackEnable(0, true); // 아이들 상태
	m_pSkinnedAnimationController->SetTrackEnable(1, false); // 달리기
	m_pSkinnedAnimationController->SetTrackEnable(6, false);
	m_pSkinnedAnimationController->SetTrackPosition(1, 0);
	m_pSkinnedAnimationController->SetTrackPosition(6, 0);
}

void CEmployee::SetRunAnimTrack()
{
	if (m_pSkinnedAnimationController == nullptr) return;
	m_pSkinnedAnimationController->SetTrackEnable(0, false); 
	m_pSkinnedAnimationController->SetTrackEnable(1, true); // 달리기
	m_pSkinnedAnimationController->SetTrackEnable(6, false);
	m_pSkinnedAnimationController->SetTrackPosition(0, 0);
	m_pSkinnedAnimationController->SetTrackPosition(1, 0);
	m_pSkinnedAnimationController->SetTrackPosition(6, 0);
}



void CEmployee::AnimTrackUpdate()
{
	switch (m_behavior)
	{
	case IDLE:
		SetIdleAnimTrack();
		break;
	case RUN:
		SetRunAnimTrack();
		break;
	case SWITCH_INTER:
		SetInteractionAnimTrack();
		break;
	case ATTACK:
	case RUN_ATTACK:
	{
		
	}
	break;
	}
}

void CEmployee::SetInteractionAnimTrackOtherClient()
{
	if (m_pSkinnedAnimationController == nullptr) return;
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


// 04-29 직원 키입력 처리 추가
void CEmployee::ProcessInput(const int16& inputKey)
{

	if (IsPlayerCanSwitchInteraction()) //  플레이어가 스위치 영역에 있는 경우
	{
		int32 switchIdx = GetAvailableSwitchIdx(); // 어느 스위치 영역인지 확인한다.
		CGenerator* targetGenerator = nullptr;
		if (switchIdx != -1) targetGenerator = mainGame.m_pScene->m_ppSwitches[switchIdx]; // 2. 활성화 가능한 스위치가 주변에 있다면
		else return;

		if (HIBYTE(inputKey) & KEY_F) // 1. 상호작용 키가 눌려있을 때 
		{
			// 1-1. 타겟 스위치 정보를 가져온다
			targetGenerator->m_lock.lock();
			if (!targetGenerator->m_bOtherPlayerInteractionOn) // 다른 플레이어가 상호작용 상태가 아니라면
			{
				if (InputManager::GetInstance().GetKeyBuffer(KEY_TYPE::F) == (int8)KEY_STATUS::KEY_PRESS &&
					!targetGenerator->m_bSwitchActive) // 플레이어가 상호작용 상태가 아니였다면
				{
					// ====== 플레이어 처리 ==============
					SetInteractionAnimation(true); // 캐릭터 상호작용 애니메이션 재생을 활성화 한다.
					

					// ========== 발전기 처리 ===============
					targetGenerator->InteractAnimation(true); // 발전기 애니메이션 재생을 시작한다.
					targetGenerator->SetAnimationCount(BUTTON_ANIM_FRAME);

					// ==========  패킷 송신 처리 ==================
					// 키가 한번 입력 됐을 때만 호출하는 것
					SC_EVENTPACKET packet;
					packet.eventId = switchIdx + (int32)EVENT_TYPE::SWITCH_ONE_START_EVENT;
					packet.size = sizeof(SC_EVENTPACKET);
					packet.type = SC_PACKET_TYPE::GAMEEVENT;
					clientCore._client->DoSend(&packet);
					
				}
			}
		}
		else // 상호작용 키가 눌러있지 않은 경우
		{
				// 키가 누르다 때졌을 때만 처리해야하는 처리들
			if (InputManager::GetInstance().GetKeyBuffer(KEY_TYPE::F) == (int8)KEY_STATUS::KEY_UP)
			{
				// ======== 플레이어 처리 ===============
				SetInteractionAnimation(false);

				// =======  발전기 처리 =================
				targetGenerator->m_lock.lock();
				targetGenerator->InteractAnimation(false); // 애니메이션 재생을 정지한다.
				targetGenerator->m_lock.unlock();
				
				//========= 패킷 송신 처리 ==============
				SC_EVENTPACKET packet;
				packet.eventId = switchIdx + (int32)EVENT_TYPE::SWITCH_ONE_END_EVENT;;
				packet.size = sizeof(SC_EVENTPACKET);
				packet.type = SC_PACKET_TYPE::GAMEEVENT;
				clientCore._client->DoSend(&packet);
			}
		}
	}
	Move(inputKey, PLAYER_VELOCITY);
}