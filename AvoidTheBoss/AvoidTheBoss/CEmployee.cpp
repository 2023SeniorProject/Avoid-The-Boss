#include "pch.h"
#include "clientIocpCore.h"
#include "GameFramework.h"
#include "InputManager.h"



CEmployee::CEmployee(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CHARACTER_TYPE nType)
{
	m_ctype = (uint8)PLAYER_TYPE::EMPLOYEE;
	m_nCharacterType = nType;

	m_pCamera = ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);
	m_InteractionCountTime = 40;

	CLoadedModelInfo* pEmployeeModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, g_pstrCharactorRefernece[(int)m_nCharacterType], NULL, Layout::PLAYER);
	SetChild(pEmployeeModel->m_pModelRootObject, true);

	m_pSkinnedAnimationController = new CAnimationController(pd3dDevice, pd3dCommandList, 7, pEmployeeModel);

	m_pSkinnedAnimationController->SetTrackAnimationSet(0, 2);//idle
	m_pSkinnedAnimationController->SetTrackAnimationSet(1, 3);//run
	m_pSkinnedAnimationController->SetTrackAnimationSet(2, 0);//faint_down (피격)
	m_pSkinnedAnimationController->SetTrackAnimationSet(3, 1);//down_idle (기어가기)
	m_pSkinnedAnimationController->SetTrackAnimationSet(4, 4);//slow_walk (절뚝거리기)
	m_pSkinnedAnimationController->SetTrackAnimationSet(5, 5);//awake (일어나기)
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

// 04-29 직원 키입력 처리 추가
void CEmployee::ProcessInput(const int16& inputKey)
{
	
	int32 pIdx = GetAvailEMPldx();
	int32 genIdx = GetAvailGenIdx(); // 어느 스위치 영역인지 확인한다.
	CGenerator* targetGenerator = mainGame.m_pScene->GetSceneGenerator(genIdx);
	
	// 발전기 상호작용 관련 인풋 처리
	
	if (IsMovable())
	{
		if (inputKey & KEY_F) // 1. 상호작용 키가 눌려있을 때 
		{
			if (GetIsInGenArea()) // 발전기 주변에 있었다면
			{
				// ====== 플레이어 처리 ==============

				// ========== 발전기 처리 ===============
				if (targetGenerator)
				{
					SetInteractionOn(true); // 캐릭터 상호작용 애니메이션 재생을 활성화 한다.
					targetGenerator->SetInteractionOn(true); // 발전기 애니메이션 재생을 시작한다.
					targetGenerator->SetAnimationCount(BUTTON_ANIM_FRAME);


					// ==========  패킷 송신 처리 ==================
					// 키가 한번 입력 됐을 때만 호출하는 것
					if (InputManager::GetInstance().GetKeyBuffer(KEY_TYPE::F) == (int8)KEY_STATUS::KEY_PRESS)
					{
						SC_EVENTPACKET packet;
						packet.eventId = genIdx + (int32)EVENT_TYPE::SWITCH_ONE_START_EVENT;
						packet.size = sizeof(SC_EVENTPACKET);
						packet.type = (uint8)SC_PACKET_TYPE::GAMEEVENT;
						clientCore._client->DoSend(&packet);
					}
				}
			}

		}
		else // 상호작용 키가 눌러있지 않은 경우
		{
			// 키가 누르다 때졌을 때만 처리해야하는 처리들
			if (InputManager::GetInstance().GetKeyBuffer(KEY_TYPE::F) == (int8)KEY_STATUS::KEY_UP)
			{

				// ======== 플레이어 처리 ===============
				SetInteractionOn(false);
				m_behavior = (uint8)PLAYER_BEHAVIOR::IDLE;
				// =======  발전기 처리 =================
				if (targetGenerator)
				{
					if (targetGenerator->m_bOnInteraction)
					{
						targetGenerator->SetInteractionOn(false); // 애니메이션 재생을 정지한다.
						//========= 패킷 송신 처리 ==============
						SC_EVENTPACKET packet;
						packet.eventId = genIdx + (int32)EVENT_TYPE::SWITCH_ONE_END_EVENT;;
						packet.size = sizeof(SC_EVENTPACKET);
						packet.type = (uint8)SC_PACKET_TYPE::GAMEEVENT;
						clientCore._client->DoSend(&packet);
					}
				}
			}
		}

		// 플레이어 살리기 상호작용
		if (inputKey & KEY_E)
		{
			// 구하는 이벤트에 관한 패킷을 전송하도록 한다.
			if (InputManager::GetKeyBuffer(KEY_TYPE::E) == (int8)KEY_STATUS::KEY_PRESS && pIdx != -1)
			{
				SC_EVENTPACKET packet;
				packet.eventId = pIdx + (int32)EVENT_TYPE::RESCUE_PLAYER_ONE;
				packet.size = sizeof(SC_EVENTPACKET);
				packet.type = (uint8)SC_PACKET_TYPE::GAMEEVENT;
				clientCore._client->DoSend(&packet);

				static_cast<CEmployee*>(mainGame.m_pScene->GetScenePlayerByIdx(pIdx))->RescueOn(true);
				std::cout << pIdx << " Rescuing\n";

			}

		}
		else
		{
			if (InputManager::GetKeyBuffer(KEY_TYPE::E) == (int8)KEY_STATUS::KEY_UP && pIdx != -1)
			{
				SC_EVENTPACKET packet;
				packet.eventId = pIdx + (int32)EVENT_TYPE::RESCUE_CANCEL_PLAYER_ONE;
				packet.size = sizeof(SC_EVENTPACKET);
				packet.type = (uint8)SC_PACKET_TYPE::GAMEEVENT;
				clientCore._client->DoSend(&packet);
				static_cast<CEmployee*>(mainGame.m_pScene->GetScenePlayerByIdx(pIdx))->ResetRescueGuage();

			}
		}
		Move(inputKey, EMPLOYEE_VELOCITY);
	}
}
void CEmployee::Move(const int16& dwDirection, float fDistance)
{

	if (m_bOnInteraction)
	{
		m_behavior = (int32)PLAYER_BEHAVIOR::SWITCH_INTER;
	}

	if (!IsMovable())
	{
		CPlayer::Move(dwDirection, 0);
		return;
	}


	// Attacked 됐을 경우 
	if (m_behavior == (int32)PLAYER_BEHAVIOR::ATTACKED)
	{
		CPlayer::Move(dwDirection, EMPLOYEE_VELOCITY); // 일시적으로 속도 느려지게 하기
		return;
	}
	
	// 플레이어의 행동을 저장.
	if (LOBYTE(dwDirection)) m_behavior = (int32)PLAYER_BEHAVIOR::RUN;
	else if (!LOBYTE(dwDirection)) m_behavior = (int32)PLAYER_BEHAVIOR::IDLE;

	// 플레이어 속도 셋팅
	CPlayer::Move(dwDirection, EMPLOYEE_VELOCITY);
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
	if (GetAvailGenIdx() != -1) m_bIsInGenArea = true;
	else m_bIsInGenArea = false;
	//======= 상호작용 가능 유저 판별 ==========

	if (m_bIsRescuing)
	{
		m_curGuage += m_rVel * fTimeElapsed;
		if (m_curGuage >= m_maxRGuage)
		{
			std::cout << "Alive\n";
			m_curGuage = 0;
			m_hp = 5;
			m_bIsRescuing = false;
			m_behavior = (int32)PLAYER_BEHAVIOR::STAND;
			m_standAnimationCount = EMPLOYEE_STAND_TIME;

			SC_EVENTPACKET packet;
			packet.type = (uint8)SC_PACKET_TYPE::GAMEEVENT;
			packet.size = sizeof(SC_EVENTPACKET);
			packet.eventId = (int32)EVENT_TYPE::ALIVE_PLAYER_ONE + m_idx;
			clientCore._client->DoSend(&packet);
		}
	}

	
	if (ptype == CLIENT_TYPE::OWNER) m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	else if (ptype == CLIENT_TYPE::OTHER_PLAYER)
	{
		if (m_behavior != (int32)PLAYER_BEHAVIOR::SWITCH_INTER) SetInteractionOn(false);
	}
}

// ===============애니메이션 트랙 ==================

void CEmployee::SetIdleAnimTrack()
{
	if (m_pSkinnedAnimationController == nullptr) return;
	m_pSkinnedAnimationController->SetTrackEnable(0, true);
	m_pSkinnedAnimationController->SetTrackEnable(1, false);
	m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_pSkinnedAnimationController->SetTrackEnable(3, false);
	m_pSkinnedAnimationController->SetTrackEnable(4, false);
	m_pSkinnedAnimationController->SetTrackEnable(5, false);
	m_pSkinnedAnimationController->SetTrackEnable(6, false);

	m_pSkinnedAnimationController->SetTrackPosition(0, 0);
	m_pSkinnedAnimationController->SetTrackPosition(1, 0);
	m_pSkinnedAnimationController->SetTrackPosition(2, 0);
	m_pSkinnedAnimationController->SetTrackPosition(3, 0);
	m_pSkinnedAnimationController->SetTrackPosition(4, 0);
	m_pSkinnedAnimationController->SetTrackPosition(5, 0);
	m_pSkinnedAnimationController->SetTrackPosition(6, 0);
}

void CEmployee::SetRunAnimTrack()
{
	if (m_pSkinnedAnimationController == nullptr) return;
	m_pSkinnedAnimationController->SetTrackEnable(0, false);
	m_pSkinnedAnimationController->SetTrackEnable(1, true);
	m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_pSkinnedAnimationController->SetTrackEnable(3, false);
	m_pSkinnedAnimationController->SetTrackEnable(4, false);
	m_pSkinnedAnimationController->SetTrackEnable(5, false);
	m_pSkinnedAnimationController->SetTrackEnable(6, false);

	m_pSkinnedAnimationController->SetTrackPosition(0, 0);
	m_pSkinnedAnimationController->SetTrackPosition(1, 0);
	m_pSkinnedAnimationController->SetTrackPosition(2, 0);
	m_pSkinnedAnimationController->SetTrackPosition(3, 0);
	m_pSkinnedAnimationController->SetTrackPosition(4, 0);
	m_pSkinnedAnimationController->SetTrackPosition(5, 0);
	m_pSkinnedAnimationController->SetTrackPosition(6, 0);
}

void CEmployee::SetDownAnimTrack()
{
	if (m_pSkinnedAnimationController == nullptr) return;
	m_pSkinnedAnimationController->SetTrackEnable(0, false);
	m_pSkinnedAnimationController->SetTrackEnable(1, false);
	m_pSkinnedAnimationController->SetTrackEnable(2, true);
	m_pSkinnedAnimationController->SetTrackEnable(3, false);
	m_pSkinnedAnimationController->SetTrackEnable(4, false);
	m_pSkinnedAnimationController->SetTrackEnable(5, false);
	m_pSkinnedAnimationController->SetTrackEnable(6, false);

	m_pSkinnedAnimationController->SetTrackPosition(0, 0);
	m_pSkinnedAnimationController->SetTrackPosition(1, 0);
	m_pSkinnedAnimationController->SetTrackPosition(2, 0);
	m_pSkinnedAnimationController->SetTrackPosition(3, 0);
	m_pSkinnedAnimationController->SetTrackPosition(4, 0);
	m_pSkinnedAnimationController->SetTrackPosition(5, 0);
	m_pSkinnedAnimationController->SetTrackPosition(6, 0);
}

void CEmployee::SetCrawlAnimTrack()
{
	if (m_pSkinnedAnimationController == nullptr) return;
	m_pSkinnedAnimationController->SetTrackEnable(0, false);
	m_pSkinnedAnimationController->SetTrackEnable(1, false);
	m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_pSkinnedAnimationController->SetTrackEnable(3, true); // CRAWL
	m_pSkinnedAnimationController->SetTrackEnable(4, false);
	m_pSkinnedAnimationController->SetTrackEnable(5, false);
	m_pSkinnedAnimationController->SetTrackEnable(6, false);

	m_pSkinnedAnimationController->SetTrackPosition(0, 0);
	m_pSkinnedAnimationController->SetTrackPosition(1, 0);
	m_pSkinnedAnimationController->SetTrackPosition(2, 0);
	m_pSkinnedAnimationController->SetTrackPosition(3, 0);
	m_pSkinnedAnimationController->SetTrackPosition(4, 0);
	m_pSkinnedAnimationController->SetTrackPosition(5, 0);
	m_pSkinnedAnimationController->SetTrackPosition(6, 0);
}

void CEmployee::SetAttackedAnimTrack()
{
	if (m_pSkinnedAnimationController == nullptr) return;
	m_pSkinnedAnimationController->SetTrackEnable(0, false);
	m_pSkinnedAnimationController->SetTrackEnable(1, false);
	m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_pSkinnedAnimationController->SetTrackEnable(3, false);
	m_pSkinnedAnimationController->SetTrackEnable(4, true);
	m_pSkinnedAnimationController->SetTrackEnable(5, false);
	m_pSkinnedAnimationController->SetTrackEnable(6, false);

	m_pSkinnedAnimationController->SetTrackPosition(0, 0);
	m_pSkinnedAnimationController->SetTrackPosition(1, 0);
	m_pSkinnedAnimationController->SetTrackPosition(2, 0);
	m_pSkinnedAnimationController->SetTrackPosition(3, 0);
	m_pSkinnedAnimationController->SetTrackPosition(4, 0);
	m_pSkinnedAnimationController->SetTrackPosition(5, 0);
	m_pSkinnedAnimationController->SetTrackPosition(6, 0);
}

void CEmployee::SetStandAnimTrack()
{
	if (m_pSkinnedAnimationController == nullptr) return;
	m_pSkinnedAnimationController->SetTrackEnable(0, false);
	m_pSkinnedAnimationController->SetTrackEnable(1, false);
	m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_pSkinnedAnimationController->SetTrackEnable(3, false);
	m_pSkinnedAnimationController->SetTrackEnable(4, false);
	m_pSkinnedAnimationController->SetTrackEnable(5, true);
	m_pSkinnedAnimationController->SetTrackEnable(6, false);

	m_pSkinnedAnimationController->SetTrackPosition(0, 0);
	m_pSkinnedAnimationController->SetTrackPosition(1, 0);
	m_pSkinnedAnimationController->SetTrackPosition(2, 0);
	m_pSkinnedAnimationController->SetTrackPosition(3, 0);
	m_pSkinnedAnimationController->SetTrackPosition(4, 0);
	m_pSkinnedAnimationController->SetTrackPosition(5, 0);
	m_pSkinnedAnimationController->SetTrackPosition(6, 0);
}

void CEmployee::SetInteractionAnimTrack()
{
	if (m_pSkinnedAnimationController == nullptr) return;
	m_pSkinnedAnimationController->SetTrackEnable(0, false);
	m_pSkinnedAnimationController->SetTrackEnable(1, false);
	m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_pSkinnedAnimationController->SetTrackEnable(3, false);
	m_pSkinnedAnimationController->SetTrackEnable(4, false);
	m_pSkinnedAnimationController->SetTrackEnable(5, false);
	m_pSkinnedAnimationController->SetTrackEnable(6, true);

	m_pSkinnedAnimationController->SetTrackPosition(0, 0);
	m_pSkinnedAnimationController->SetTrackPosition(1, 0);
	m_pSkinnedAnimationController->SetTrackPosition(2, 0);
	m_pSkinnedAnimationController->SetTrackPosition(3, 0);
	m_pSkinnedAnimationController->SetTrackPosition(4, 0);
	m_pSkinnedAnimationController->SetTrackPosition(5, 0);
	m_pSkinnedAnimationController->SetTrackPosition(6, 0);
}

void CEmployee::AnimTrackUpdate()
{
	switch (m_behavior)
	{
	case (int32)PLAYER_BEHAVIOR::IDLE:
		SetIdleAnimTrack();
		break;
	case (int32)PLAYER_BEHAVIOR::RUN:
		SetRunAnimTrack();
		break;
	case (int32)PLAYER_BEHAVIOR::SWITCH_INTER:
		SetInteractionAnimTrack();
		break;
	case (int32)PLAYER_BEHAVIOR::ATTACKED:
		if (m_attackedAnimationCount == EMPLOYEE_ATTACKED_TIME)
		{
			SetAttackedAnimTrack();
			m_attackedAnimationCount--;
		}
		else 
		{
			m_attackedAnimationCount--;
			m_behavior = (int32)PLAYER_BEHAVIOR::ATTACKED;
			if (m_attackedAnimationCount == 0)
			{
				m_behavior = (int32)PLAYER_BEHAVIOR::IDLE;
			}
		}
		break;
	case (int32)PLAYER_BEHAVIOR::DOWN:
		if (m_downAnimationCount == EMPLOYEE_DOWN_TIME)
		{
			SetDownAnimTrack();
			m_downAnimationCount--;
		}
		else
		{
			m_downAnimationCount--;
			if (m_downAnimationCount <= 0)
			{
				m_behavior = (int32)PLAYER_BEHAVIOR::CRAWL;
			}
		}
		break;
	case (int32)PLAYER_BEHAVIOR::CRAWL:
		SetCrawlAnimTrack();
		break;
	case (int32)PLAYER_BEHAVIOR::STAND:
		if (m_standAnimationCount == EMPLOYEE_STAND_TIME)
		{
			SetStandAnimTrack();
			m_standAnimationCount--;
		}
		else
		{
			m_standAnimationCount--;
			if (m_standAnimationCount <= 0)
			{
				m_behavior = (int32)PLAYER_BEHAVIOR::IDLE;
			}
		}
		break;
	}
}

void CEmployee::SetInteractionAnimTrackOtherClient()
{
	if (m_pSkinnedAnimationController == nullptr) return;
	if (m_bOnInteraction)
	{
		m_pSkinnedAnimationController->SetTrackEnable(0, false);
		m_pSkinnedAnimationController->SetTrackEnable(1, false);
		m_pSkinnedAnimationController->SetTrackEnable(2, false);
		m_pSkinnedAnimationController->SetTrackEnable(3, false);
		m_pSkinnedAnimationController->SetTrackEnable(4, false);
		m_pSkinnedAnimationController->SetTrackEnable(5, false);
		m_pSkinnedAnimationController->SetTrackEnable(6, true);

		m_pSkinnedAnimationController->SetTrackPosition(0, 0);
		m_pSkinnedAnimationController->SetTrackPosition(1, 0);
		m_pSkinnedAnimationController->SetTrackPosition(2, 0);
		m_pSkinnedAnimationController->SetTrackPosition(3, 0);
		m_pSkinnedAnimationController->SetTrackPosition(4, 0);
		m_pSkinnedAnimationController->SetTrackPosition(5, 0);
		m_pSkinnedAnimationController->SetTrackPosition(6, 0);
	}
}

int32 CEmployee::GetAvailGenIdx()
{
	for (int i = 0; i < 3; ++i)
	{
		XMFLOAT3 distanceVec = Vector3::Subtract(m_xmf3Position, m_pSwitches[i].position);
		float distance = Vector3::Length(distanceVec);
		float sumRange = m_playerBV.Radius + m_pSwitches[i].radius;
		if (distance <= sumRange)
		{
			CGenerator* targetGenerator = mainGame.m_pScene->GetSceneGenerator(i);
			if (targetGenerator)
			{
				if (targetGenerator->IsAvailable()) return i;
			}
		}
	}
	
	return -1;
}

int32 CEmployee::GetAvailEMPldx()
{
	for (int i = 1; i < PLAYERNUM; ++i)
	{

		CEmployee* p = static_cast<CEmployee*>(mainGame.m_pScene->GetScenePlayerByIdx(i));
		if (p == nullptr || i == m_idx) continue;
		XMFLOAT3 ppos = p->GetPosition();
		ppos = Vector3::Subtract(m_xmf3Position, ppos);
		float dist = Vector3::Length(ppos);
		if (dist < 1.5 && p->GetPlayerBehavior() == (int32)PLAYER_BEHAVIOR::CRAWL && !p->GetRescueOn()) return i;
	}
	return -1;
}

// ============== 플레이어 상태 변경 처리 ============ 05-23
void CEmployee::PlayerAttacked()
{
	if (m_hp > 0)
	{
		m_hp -= 1;
		if (m_hp == 0)
		{
			PlayerDown();
		}
		else
		{
			m_behavior = (int32)PLAYER_BEHAVIOR::ATTACKED;
			m_attackedAnimationCount = EMPLOYEE_ATTACKED_TIME;
		}
		
	}
	
}

void CEmployee::PlayerDown()
{
	m_behavior = (int32)PLAYER_BEHAVIOR::DOWN;
	m_downAnimationCount = EMPLOYEE_DOWN_TIME;
}

