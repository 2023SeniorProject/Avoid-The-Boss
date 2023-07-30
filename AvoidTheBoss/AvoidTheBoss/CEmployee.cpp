#include "pch.h"
#include "CEmployee.h"
#include "clientIocpCore.h"
#include "GameFramework.h"

#include "CGenerator.h"
#include "CSound.h"

#include "GameScene.h"
#include "OtherScenes.h"

#include "InputManager.h"
#include "SceneManager.h"
#include "SoundManager.h"
#include "UIManager.h"

CEmployee::CEmployee(ID3D12Device5* pd3dDevice, ID3D12GraphicsCommandList4* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CHARACTER_TYPE nType)
{
	m_ctype = (uint8)PLAYER_TYPE::EMPLOYEE;
	m_nCharacterType = nType;
	m_pCamera = ChangeCamera(FIRST_PERSON_CAMERA, 0.0f);

	// 1 인칭 애니메이션 로드	
		//달리기, 버튼, 느리게 걷기, 대기
		CLoadedModelInfo* pEmployeeModel1v = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, g_pstrFirstCharactorRefernece[(int)m_nCharacterType], NULL, Layout::PLAYER);
		SetChild(pEmployeeModel1v->m_pModelRootObject, true);

		m_pSkinnedAnimationController2 = new CAnimationController(pd3dDevice, pd3dCommandList, 4, pEmployeeModel1v);
		m_pSkinnedAnimationController2->SetTrackAnimationSet(0, 3);//idle
		m_pSkinnedAnimationController2->SetTrackAnimationSet(1, 0);//run
		m_pSkinnedAnimationController2->SetTrackAnimationSet(2, 2);//slow_walk (절뚝거리기)
		m_pSkinnedAnimationController2->SetTrackAnimationSet(3, 1);//button

	// 3인칭 애니메이션 로드
		CLoadedModelInfo* pEmployeeModel3v = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, g_pstrThirdCharactorRefernece[(int)m_nCharacterType], NULL, Layout::PLAYER);
		SetChild(pEmployeeModel3v->m_pModelRootObject, true);

		m_pSkinnedAnimationController1 = new CAnimationController(pd3dDevice, pd3dCommandList, 7, pEmployeeModel3v);

		//필요없는 애니메이션
		m_pSkinnedAnimationController1->SetTrackAnimationSet(0, 2);//idle x
		m_pSkinnedAnimationController1->SetTrackAnimationSet(1, 3);//run x
		m_pSkinnedAnimationController1->SetTrackAnimationSet(2, 0);//down (총알 맞고 쓰러짐) x 
		m_pSkinnedAnimationController1->SetTrackAnimationSet(3, 1);//down_idle,crawl (쓰러진 상태) ㅇ
		m_pSkinnedAnimationController1->SetTrackAnimationSet(4, 4);//slow_walk (절뚝거리기) x
		m_pSkinnedAnimationController1->SetTrackAnimationSet(5, 5);//stand (쓰러진 상태에서 일어나기) ㅇ
		m_pSkinnedAnimationController1->SetTrackAnimationSet(6, 6);//button ㅇ
		m_pSkinnedAnimationController1->SetTrackAnimationSet(7, 7);//만세 X
	

	if (m_pCamera->m_nMode == (DWORD)FIRST_PERSON_CAMERA)
		//달리기, 버튼, 느리게 걷기, 대기
	{
		m_pSkinnedAnimationController2->SetTrackEnable(0, false);
		m_pSkinnedAnimationController2->SetTrackEnable(1, false);
		m_pSkinnedAnimationController2->SetTrackEnable(2, false);
		m_pSkinnedAnimationController2->SetTrackEnable(3, true);

		m_pSkinnedAnimationController1->SetTrackEnable(0, false);
		m_pSkinnedAnimationController1->SetTrackEnable(1, false);
		m_pSkinnedAnimationController1->SetTrackEnable(2, false);
		m_pSkinnedAnimationController1->SetTrackEnable(3, false);
		m_pSkinnedAnimationController1->SetTrackEnable(4, false);
		m_pSkinnedAnimationController1->SetTrackEnable(5, false);
		m_pSkinnedAnimationController1->SetTrackEnable(6, false);
		m_pSkinnedAnimationController1->SetTrackEnable(7, false);
	}
	if (m_pCamera->m_nMode == (DWORD)THIRD_PERSON_CAMERA)
	{
		m_pSkinnedAnimationController2->SetTrackEnable(0, false);
		m_pSkinnedAnimationController2->SetTrackEnable(1, false);
		m_pSkinnedAnimationController2->SetTrackEnable(2, false);
		m_pSkinnedAnimationController2->SetTrackEnable(3, false);

		m_pSkinnedAnimationController1->SetTrackEnable(0, true); // idle
		m_pSkinnedAnimationController1->SetTrackEnable(1, false);
		m_pSkinnedAnimationController1->SetTrackEnable(2, false);
		m_pSkinnedAnimationController1->SetTrackEnable(3, false);
		m_pSkinnedAnimationController1->SetTrackEnable(4, false);
		m_pSkinnedAnimationController1->SetTrackEnable(5, false);
		m_pSkinnedAnimationController1->SetTrackEnable(7, false);

	}
	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	if (pEmployeeModel1v)delete pEmployeeModel1v;
	if (pEmployeeModel3v)delete pEmployeeModel3v;
}

CEmployee::~CEmployee()
{
	delete[] m_pSwitches;
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
	m_pCamera->SetPosition(m_xmf3Position);
	m_pCamera->SetPosition(Vector3::Add(m_xmf3Position,m_pCamera->GetOffset()));
	m_pCamera->m_fogOn = true;
	Update(fTimeElapsed, m_clientType);

	return(m_pCamera);
}

// 04-29 직원 키입력 처리 추가
// 입력 처리 및 플레이어의 행동을 미리 셋팅한다.
uint8 CEmployee::ProcessInput()
{
	// 발전기 상호작용 관련 인풋 처리

	uint8 dir = 0;
	if (!IsSeMiBehavior() && !IsMovable())
	{
		if (InputManager::GetInstance().GetKeyBuffer(KEY_TYPE::W) > 0)  dir |= KEY_FORWARD;
		if (InputManager::GetInstance().GetKeyBuffer(KEY_TYPE::A) > 0)  dir |= KEY_LEFT;
		if (InputManager::GetInstance().GetKeyBuffer(KEY_TYPE::S) > 0)  dir |= KEY_BACKWARD;
		if (InputManager::GetInstance().GetKeyBuffer(KEY_TYPE::D) > 0)  dir |= KEY_RIGHT;

		if (dir) SetBehavior(PLAYER_BEHAVIOR::RUN);
		else	 SetBehavior(PLAYER_BEHAVIOR::IDLE);

		// 구조 작업이나 발전기 상호작용을 수행하고 있다면 
		
		
	}

	if (InputManager::GetInstance().GetKeyBuffer(KEY_TYPE::G) == (uint8)KEY_STATUS::KEY_PRESS)
	{
		if (m_pCamera->m_fogOn) m_pCamera->m_fogOn = false;
		else m_pCamera->m_fogOn = true;
	}

	GenTasking();
	RescueTasking();

	Move(dir, EMPLOYEE_VELOCITY);
	return dir;
	
}

void CEmployee::Move(const int16& dwDirection, float fDistance)
{
	if (m_clientType == CLIENT_TYPE::OTHER_PLAYER)
	{
		if (false == IsSeMiBehavior())
		{
			if ((int32)PLAYER_BEHAVIOR::ATTACKED == GetBehavior() && m_attackedAnimationCount >= 0);
			else if (LOBYTE(dwDirection)) SetBehavior(PLAYER_BEHAVIOR::RUN);
			else if(!LOBYTE(dwDirection)) SetBehavior(PLAYER_BEHAVIOR::IDLE);
		}
	}

	switch (GetBehavior())
	{
	case (int32)PLAYER_BEHAVIOR::RESCUE:
	case (int32)PLAYER_BEHAVIOR::STAND:
	case (int32)PLAYER_BEHAVIOR::DOWN:
	case (int32)PLAYER_BEHAVIOR::CRAWL:
	case (int32)PLAYER_BEHAVIOR::EXIT:
		CPlayer::Move(0, 0);
		break;
	case (int32)PLAYER_BEHAVIOR::IDLE:
	case (int32)PLAYER_BEHAVIOR::RUN:
	case (int32)PLAYER_BEHAVIOR::ATTACKED:
		CPlayer::Move(dwDirection, EMPLOYEE_VELOCITY);
		break;
	}
	
}

void CEmployee::Update(float fTimeElapsed, CLIENT_TYPE ptype)
{
	// 일단 주인이면 기본적으로 3인칭 렌더링을 수행한다.
	if (CLIENT_TYPE::OWNER == m_clientType)
	{
		m_IsFirst = true;
	}
	// 근데 만약 다른 플레이어거나, 주인이 3인칭 카메라로 시점을 변경하게 된다면 삭제한다.
	if (CLIENT_TYPE::OTHER_PLAYER == m_clientType || THIRD_PERSON_CAMERA == m_pCamera->GetMode())
		m_IsFirst = false;
	
	CPlayer::Update(fTimeElapsed, ptype);
	LateUpdate(fTimeElapsed,ptype);
}

void CEmployee::LateUpdate(float fTimeElapsed, CLIENT_TYPE ptype)
{
	// ===== 애니메이션 트랙 업데이트 ========
	AnimTrackUpdate();
	
	// 무적시간 동안 피격 이펙트 재생
	if (m_bIsInvincibility)
	{
		m_UICoolTime -= fTimeElapsed;
	}
	else 
	{
		m_UICoolTime = 1.0f;
	}
	
	// 내가 구해짐을 받고 있다면~
	if (m_bIsRescuing)
	{
		m_curGuage += m_rVel * fTimeElapsed;
		if (m_curGuage >= m_maxRGuage)
		{
			
			m_curGuage = 0;
			m_hp = 3;
			m_bIsRescuing = false;
			
			SetBehavior(PLAYER_BEHAVIOR::STAND);
			m_standAnimationCount = EMPLOYEE_STAND_TIME;

			SC_EVENTPACKET packet;
			packet.type = (uint8)SC_GAME_PACKET_TYPE::GAMEEVENT;
			packet.size = sizeof(SC_EVENTPACKET);
			packet.eventId = (int32)EVENT_TYPE::ALIVE_PLAYER_ONE + m_idx;
			clientCore.DoSend(&packet);
		}
	}

	// 탈출 후 맵에서 일정 범위 이상 넘어가게 되면 EXIT 상태로 만세 애니메이션 재생
	if (!m_bEmpExit && static_cast<CGameScene*>(mainGame.m_SceneManager->GetSceneByIdx(3))->m_bEmpExit && m_clientType == CLIENT_TYPE::OWNER)
	{
		// 결과 씬에 넘겨주기
		static_cast<CResultScene*>(mainGame.m_SceneManager->GetSceneByIdx(4))->m_activeCnt = m_activeCnt;
		static_cast<CResultScene*>(mainGame.m_SceneManager->GetSceneByIdx(4))->m_deadCnt = m_deadCnt;
\
		ChangeCamera(FIRST_PERSON_CAMERA, 0);
		mainGame.m_SceneManager->GetSceneByIdx(3)->m_pCamera = m_pCamera;
		mainGame.m_SceneManager->GetSceneByIdx(3)->m_pCamera->CreateShaderVariables(mainGame.m_pd3dDevice, mainGame.m_pd3dCommandList);
		
		if (GetPosition().x < -28 || GetPosition().x > 28 || GetPosition().z > 28 || GetPosition().z < -28)
		{
			m_bEmpExit = true;
			SetBehavior(PLAYER_BEHAVIOR::EXIT);
			
			SC_EVENTPACKET packet;
			packet.size = sizeof(SC_EVENTPACKET);
			packet.type = (uint8)SC_GAME_PACKET_TYPE::GAMEEVENT;
			packet.eventId = m_idx + (uint8)EVENT_TYPE::EXIT_PLAYER_ONE;
			
			clientCore.DoSend(&packet); // 탈출 시 전송
		}
	}

	if (ptype == CLIENT_TYPE::OWNER) m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	else if (ptype == CLIENT_TYPE::OTHER_PLAYER)
	{
		if (m_behavior != (int32)PLAYER_BEHAVIOR::SWITCH_INTER) SetGenInteraction(false);
		if (m_behavior == (int32)PLAYER_BEHAVIOR::EXIT) m_hide = true;
	}
}

// ===============애니메이션 트랙 ==================
// 애니메이션 1인칭인덱스 3인칭인덱스 
// 걷기 0
void CEmployee::SetIdleAnimTrack()
{
	if (CLIENT_TYPE::OWNER == m_clientType)
	{
		if (m_pSkinnedAnimationController2 == nullptr) return;
		m_pSkinnedAnimationController2->SetTrackEnable(0, true);
		m_pSkinnedAnimationController2->SetTrackEnable(1, false);
		m_pSkinnedAnimationController2->SetTrackEnable(2, false);
		m_pSkinnedAnimationController2->SetTrackEnable(3, false);

		if (m_pSkinnedAnimationController1 == nullptr) return;
		m_pSkinnedAnimationController1->SetTrackEnable(0, false);
		m_pSkinnedAnimationController1->SetTrackEnable(1, false);
		m_pSkinnedAnimationController1->SetTrackEnable(2, false);
		m_pSkinnedAnimationController1->SetTrackEnable(3, false);
		m_pSkinnedAnimationController1->SetTrackEnable(4, false);
		m_pSkinnedAnimationController1->SetTrackEnable(5, false);
		m_pSkinnedAnimationController1->SetTrackEnable(6, false);
		m_pSkinnedAnimationController1->SetTrackEnable(7, false);
	}
	else if (CLIENT_TYPE::OTHER_PLAYER == m_clientType)
	{
		if (m_pSkinnedAnimationController2 == nullptr) return;
		m_pSkinnedAnimationController2->SetTrackEnable(0, false);
		m_pSkinnedAnimationController2->SetTrackEnable(1, false);
		m_pSkinnedAnimationController2->SetTrackEnable(2, false);
		m_pSkinnedAnimationController2->SetTrackEnable(3, false);

		if (m_pSkinnedAnimationController1 == nullptr) return;
		m_pSkinnedAnimationController1->SetTrackEnable(0, true);
		m_pSkinnedAnimationController1->SetTrackEnable(1, false);
		m_pSkinnedAnimationController1->SetTrackEnable(2, false);
		m_pSkinnedAnimationController1->SetTrackEnable(3, false);
		m_pSkinnedAnimationController1->SetTrackEnable(4, false);
		m_pSkinnedAnimationController1->SetTrackEnable(5, false);
		m_pSkinnedAnimationController1->SetTrackEnable(6, false);
		m_pSkinnedAnimationController1->SetTrackEnable(7, false);
	}

	m_pSkinnedAnimationController2->SetTrackPosition(0, 0);
	m_pSkinnedAnimationController2->SetTrackPosition(1, 0);
	m_pSkinnedAnimationController2->SetTrackPosition(2, 0);
	m_pSkinnedAnimationController2->SetTrackPosition(3, 0);

	m_pSkinnedAnimationController1->SetTrackPosition(0, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(1, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(2, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(3, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(4, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(5, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(6, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(7, 0);
}

// 달리기 1
void CEmployee::SetRunAnimTrack()
{
	if (CLIENT_TYPE::OWNER == m_clientType)
	{
		if (m_pSkinnedAnimationController2 == nullptr) return;
		m_pSkinnedAnimationController2->SetTrackEnable(0, false);
		m_pSkinnedAnimationController2->SetTrackEnable(1, true);
		m_pSkinnedAnimationController2->SetTrackEnable(2, false);
		m_pSkinnedAnimationController2->SetTrackEnable(3, false);

		if (m_pSkinnedAnimationController1 == nullptr) return;
		m_pSkinnedAnimationController1->SetTrackEnable(0, false);
		m_pSkinnedAnimationController1->SetTrackEnable(1, false);
		m_pSkinnedAnimationController1->SetTrackEnable(2, false);
		m_pSkinnedAnimationController1->SetTrackEnable(3, false);
		m_pSkinnedAnimationController1->SetTrackEnable(4, false);
		m_pSkinnedAnimationController1->SetTrackEnable(5, false);
		m_pSkinnedAnimationController1->SetTrackEnable(6, false);
		m_pSkinnedAnimationController1->SetTrackEnable(7, false);
	}
	else if (CLIENT_TYPE::OTHER_PLAYER == m_clientType)
	{
		if (m_pSkinnedAnimationController2 == nullptr) return;
		m_pSkinnedAnimationController2->SetTrackEnable(0, false);
		m_pSkinnedAnimationController2->SetTrackEnable(1, false);
		m_pSkinnedAnimationController2->SetTrackEnable(2, false);
		m_pSkinnedAnimationController2->SetTrackEnable(3, false);

		if (m_pSkinnedAnimationController1 == nullptr) return;
		m_pSkinnedAnimationController1->SetTrackEnable(0, false);
		m_pSkinnedAnimationController1->SetTrackEnable(1, true);
		m_pSkinnedAnimationController1->SetTrackEnable(2, false);
		m_pSkinnedAnimationController1->SetTrackEnable(3, false);
		m_pSkinnedAnimationController1->SetTrackEnable(4, false);
		m_pSkinnedAnimationController1->SetTrackEnable(5, false);
		m_pSkinnedAnimationController1->SetTrackEnable(6, false);
		m_pSkinnedAnimationController1->SetTrackEnable(7, false);
	}
	m_pSkinnedAnimationController2->SetTrackPosition(0, 0);
	m_pSkinnedAnimationController2->SetTrackPosition(1, 0);
	m_pSkinnedAnimationController2->SetTrackPosition(2, 0);
	m_pSkinnedAnimationController2->SetTrackPosition(3, 0);

	m_pSkinnedAnimationController1->SetTrackPosition(0, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(1, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(2, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(3, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(4, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(5, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(6, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(7, 0);
}

// 절뚝거리기 2,4
void CEmployee::SetAttackedAnimTrack()
{
	if (CLIENT_TYPE::OWNER == m_clientType)
	{
		if (m_pSkinnedAnimationController2 == nullptr) return;
		m_pSkinnedAnimationController2->SetTrackEnable(0, false);
		m_pSkinnedAnimationController2->SetTrackEnable(1, false);
		m_pSkinnedAnimationController2->SetTrackEnable(2, true);
		m_pSkinnedAnimationController2->SetTrackEnable(3, false);

		if (m_pSkinnedAnimationController1 == nullptr) return;
		m_pSkinnedAnimationController1->SetTrackEnable(0, false);
		m_pSkinnedAnimationController1->SetTrackEnable(1, false);
		m_pSkinnedAnimationController1->SetTrackEnable(2, false);
		m_pSkinnedAnimationController1->SetTrackEnable(3, false);
		m_pSkinnedAnimationController1->SetTrackEnable(4, false);
		m_pSkinnedAnimationController1->SetTrackEnable(5, false);
		m_pSkinnedAnimationController1->SetTrackEnable(6, false);
		m_pSkinnedAnimationController1->SetTrackEnable(7, false);
	}
	else if (CLIENT_TYPE::OTHER_PLAYER == m_clientType)
	{
		if (m_pSkinnedAnimationController2 == nullptr) return;
		m_pSkinnedAnimationController2->SetTrackEnable(0, false);
		m_pSkinnedAnimationController2->SetTrackEnable(1, false);
		m_pSkinnedAnimationController2->SetTrackEnable(2, false);
		m_pSkinnedAnimationController2->SetTrackEnable(3, false);

		if (m_pSkinnedAnimationController1 == nullptr) return;
		m_pSkinnedAnimationController1->SetTrackEnable(0, false);
		m_pSkinnedAnimationController1->SetTrackEnable(1, false);
		m_pSkinnedAnimationController1->SetTrackEnable(2, false);
		m_pSkinnedAnimationController1->SetTrackEnable(3, false);
		m_pSkinnedAnimationController1->SetTrackEnable(4, true);
		m_pSkinnedAnimationController1->SetTrackEnable(5, false);
		m_pSkinnedAnimationController1->SetTrackEnable(6, false);
		m_pSkinnedAnimationController1->SetTrackEnable(7, false);
	}

	m_pSkinnedAnimationController2->SetTrackPosition(0, 0);
	m_pSkinnedAnimationController2->SetTrackPosition(1, 0);
	m_pSkinnedAnimationController2->SetTrackPosition(2, 0);
	m_pSkinnedAnimationController2->SetTrackPosition(3, 0);

	m_pSkinnedAnimationController1->SetTrackPosition(0, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(1, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(2, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(3, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(4, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(5, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(6, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(7, 0);
}

// 발전기 상호작용 3,6
void CEmployee::SetInteractionAnimTrack()
{
	// 발전기 상호작용
	if (CLIENT_TYPE::OWNER == m_clientType)
	{
		if (m_pSkinnedAnimationController2 == nullptr) return;
		m_pSkinnedAnimationController2->SetTrackEnable(0, false);
		m_pSkinnedAnimationController2->SetTrackEnable(1, false);
		m_pSkinnedAnimationController2->SetTrackEnable(2, false);
		m_pSkinnedAnimationController2->SetTrackEnable(3, true);

		if (m_pSkinnedAnimationController1 == nullptr) return;
		m_pSkinnedAnimationController1->SetTrackEnable(0, false);
		m_pSkinnedAnimationController1->SetTrackEnable(1, false);
		m_pSkinnedAnimationController1->SetTrackEnable(2, false);
		m_pSkinnedAnimationController1->SetTrackEnable(3, false);
		m_pSkinnedAnimationController1->SetTrackEnable(4, false);
		m_pSkinnedAnimationController1->SetTrackEnable(5, false);
		m_pSkinnedAnimationController1->SetTrackEnable(6, false);
		m_pSkinnedAnimationController1->SetTrackEnable(7, false);
	}
	else if (CLIENT_TYPE::OTHER_PLAYER == m_clientType)
	{
		if (m_pSkinnedAnimationController2 == nullptr) return;
		m_pSkinnedAnimationController2->SetTrackEnable(0, false);
		m_pSkinnedAnimationController2->SetTrackEnable(1, false);
		m_pSkinnedAnimationController2->SetTrackEnable(2, false);
		m_pSkinnedAnimationController2->SetTrackEnable(3, false);

		if (m_pSkinnedAnimationController1 == nullptr) return;
		m_pSkinnedAnimationController1->SetTrackEnable(0, false);
		m_pSkinnedAnimationController1->SetTrackEnable(1, false);
		m_pSkinnedAnimationController1->SetTrackEnable(2, false);
		m_pSkinnedAnimationController1->SetTrackEnable(3, false);
		m_pSkinnedAnimationController1->SetTrackEnable(4, false);
		m_pSkinnedAnimationController1->SetTrackEnable(5, false);
		m_pSkinnedAnimationController1->SetTrackEnable(6, true);
		m_pSkinnedAnimationController1->SetTrackEnable(7, false);
	}
	m_pSkinnedAnimationController2->SetTrackPosition(0, 0);
	m_pSkinnedAnimationController2->SetTrackPosition(1, 0);
	m_pSkinnedAnimationController2->SetTrackPosition(2, 0);
	m_pSkinnedAnimationController2->SetTrackPosition(3, 0);
}

// 쓰러진 상태 x,3
void CEmployee::SetCrawlAnimTrack()
{
	if (CLIENT_TYPE::OWNER == m_clientType && FIRST_PERSON_CAMERA == m_pCamera->GetMode())
	{
		// 일단 아이들 상태 애니메이션 재생하도록 한다.
		if (m_pSkinnedAnimationController2 == nullptr) return;
		m_pSkinnedAnimationController2->SetTrackEnable(0, true);
		m_pSkinnedAnimationController2->SetTrackEnable(1, false);
		m_pSkinnedAnimationController2->SetTrackEnable(2, false);
		m_pSkinnedAnimationController2->SetTrackEnable(3, false);

		if (m_pSkinnedAnimationController1 == nullptr) return;
		m_pSkinnedAnimationController1->SetTrackEnable(0, false);
		m_pSkinnedAnimationController1->SetTrackEnable(1, false);
		m_pSkinnedAnimationController1->SetTrackEnable(2, false);
		m_pSkinnedAnimationController1->SetTrackEnable(3, false);
		m_pSkinnedAnimationController1->SetTrackEnable(4, false);
		m_pSkinnedAnimationController1->SetTrackEnable(5, false);
		m_pSkinnedAnimationController1->SetTrackEnable(6, false);
	}
	else if (CLIENT_TYPE::OWNER == m_clientType && THIRD_PERSON_CAMERA == m_pCamera->GetMode())
	{
		// 일단 아이들 상태 애니메이션 재생하도록 한다.
		if (m_pSkinnedAnimationController2 == nullptr) return;
		m_pSkinnedAnimationController2->SetTrackEnable(0, false);
		m_pSkinnedAnimationController2->SetTrackEnable(1, false);
		m_pSkinnedAnimationController2->SetTrackEnable(2, false);
		m_pSkinnedAnimationController2->SetTrackEnable(3, false);

		if (m_pSkinnedAnimationController1 == nullptr) return;
		m_pSkinnedAnimationController1->SetTrackEnable(0, false);
		m_pSkinnedAnimationController1->SetTrackEnable(1, false);
		m_pSkinnedAnimationController1->SetTrackEnable(2, false);
		m_pSkinnedAnimationController1->SetTrackEnable(3, true);
		m_pSkinnedAnimationController1->SetTrackEnable(4, false);
		m_pSkinnedAnimationController1->SetTrackEnable(5, false);
		m_pSkinnedAnimationController1->SetTrackEnable(6, false);
		m_pSkinnedAnimationController1->SetTrackEnable(7, false);
	}

	// 만약 그냥 다른 플레이어라면 ~
	if (CLIENT_TYPE::OTHER_PLAYER == m_clientType)
	{
		if (m_pSkinnedAnimationController2 == nullptr) return;
		m_pSkinnedAnimationController2->SetTrackEnable(0, false);
		m_pSkinnedAnimationController2->SetTrackEnable(1, false);
		m_pSkinnedAnimationController2->SetTrackEnable(2, false);
		m_pSkinnedAnimationController2->SetTrackEnable(3, false);

		if (m_pSkinnedAnimationController1 == nullptr) return;
		m_pSkinnedAnimationController1->SetTrackEnable(0, false);
		m_pSkinnedAnimationController1->SetTrackEnable(1, false);
		m_pSkinnedAnimationController1->SetTrackEnable(2, false);
		m_pSkinnedAnimationController1->SetTrackEnable(3, true);
		m_pSkinnedAnimationController1->SetTrackEnable(4, false);
		m_pSkinnedAnimationController1->SetTrackEnable(5, false);
		m_pSkinnedAnimationController1->SetTrackEnable(6, false);
		m_pSkinnedAnimationController1->SetTrackEnable(7, false);
	}

	m_pSkinnedAnimationController2->SetTrackPosition(0, 0);
	m_pSkinnedAnimationController2->SetTrackPosition(1, 0);
	m_pSkinnedAnimationController2->SetTrackPosition(2, 0);
	m_pSkinnedAnimationController2->SetTrackPosition(3, 0);

	m_pSkinnedAnimationController1->SetTrackPosition(0, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(1, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(2, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(3, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(4, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(5, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(6, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(7, 0);
}
// 총알 맞고 쓰러짐 x,2
void CEmployee::SetDownAnimTrack()
{
	if (CLIENT_TYPE::OWNER == m_clientType && FIRST_PERSON_CAMERA == m_pCamera->GetMode())
	{
		if (m_pSkinnedAnimationController2 == nullptr) return;
		m_pSkinnedAnimationController2->SetTrackEnable(0, true);
		m_pSkinnedAnimationController2->SetTrackEnable(1, false);
		m_pSkinnedAnimationController2->SetTrackEnable(2, false);
		m_pSkinnedAnimationController2->SetTrackEnable(3, false);

		if (m_pSkinnedAnimationController1 == nullptr) return;
		m_pSkinnedAnimationController1->SetTrackEnable(0, false);
		m_pSkinnedAnimationController1->SetTrackEnable(1, false);
		m_pSkinnedAnimationController1->SetTrackEnable(2, false);
		m_pSkinnedAnimationController1->SetTrackEnable(3, false);
		m_pSkinnedAnimationController1->SetTrackEnable(4, false);
		m_pSkinnedAnimationController1->SetTrackEnable(5, false);
		m_pSkinnedAnimationController1->SetTrackEnable(6, false);
		m_pSkinnedAnimationController1->SetTrackEnable(7, false);
	}
	else if (CLIENT_TYPE::OWNER == m_clientType && THIRD_PERSON_CAMERA == m_pCamera->GetMode())
	{
		// 일단 아이들 상태 애니메이션 재생하도록 한다.
		if (m_pSkinnedAnimationController2 == nullptr) return;
		m_pSkinnedAnimationController2->SetTrackEnable(0, false);
		m_pSkinnedAnimationController2->SetTrackEnable(1, false);
		m_pSkinnedAnimationController2->SetTrackEnable(2, false);
		m_pSkinnedAnimationController2->SetTrackEnable(3, false);

		if (m_pSkinnedAnimationController1 == nullptr) return;
		m_pSkinnedAnimationController1->SetTrackEnable(0, false);
		m_pSkinnedAnimationController1->SetTrackEnable(1, false);
		m_pSkinnedAnimationController1->SetTrackEnable(2, true);
		m_pSkinnedAnimationController1->SetTrackEnable(3, false);
		m_pSkinnedAnimationController1->SetTrackEnable(4, false);
		m_pSkinnedAnimationController1->SetTrackEnable(5, false);
		m_pSkinnedAnimationController1->SetTrackEnable(6, false);
		m_pSkinnedAnimationController1->SetTrackEnable(7, false);
	}


	if (CLIENT_TYPE::OTHER_PLAYER == m_clientType)
	{
		if (m_pSkinnedAnimationController2 == nullptr) return;
		m_pSkinnedAnimationController2->SetTrackEnable(0, false);
		m_pSkinnedAnimationController2->SetTrackEnable(1, false);
		m_pSkinnedAnimationController2->SetTrackEnable(2, false);
		m_pSkinnedAnimationController2->SetTrackEnable(3, false);

		if (m_pSkinnedAnimationController1 == nullptr) return;
		m_pSkinnedAnimationController1->SetTrackEnable(0, false);
		m_pSkinnedAnimationController1->SetTrackEnable(1, false);
		m_pSkinnedAnimationController1->SetTrackEnable(2, true);
		m_pSkinnedAnimationController1->SetTrackEnable(3, false);
		m_pSkinnedAnimationController1->SetTrackEnable(4, false);
		m_pSkinnedAnimationController1->SetTrackEnable(5, false);
		m_pSkinnedAnimationController1->SetTrackEnable(6, false);
		m_pSkinnedAnimationController1->SetTrackEnable(7, false);
	}
	m_pSkinnedAnimationController2->SetTrackPosition(0, 0);
	m_pSkinnedAnimationController2->SetTrackPosition(1, 0);
	m_pSkinnedAnimationController2->SetTrackPosition(2, 0);
	m_pSkinnedAnimationController2->SetTrackPosition(3, 0);

	m_pSkinnedAnimationController1->SetTrackPosition(0, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(1, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(2, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(3, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(4, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(5, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(6, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(7, 0);
}
// 일어나기 x,5
void CEmployee::SetStandAnimTrack()
{
	// 일어나기
	if (CLIENT_TYPE::OWNER == m_clientType && FIRST_PERSON_CAMERA == m_pCamera->GetMode())
	{
		if (m_pSkinnedAnimationController2 == nullptr) return;
		m_pSkinnedAnimationController2->SetTrackEnable(0, true); // Idle 애니메이션 재생하도록
		m_pSkinnedAnimationController2->SetTrackEnable(1, false);
		m_pSkinnedAnimationController2->SetTrackEnable(2, false);
		m_pSkinnedAnimationController2->SetTrackEnable(3, false);

		if (m_pSkinnedAnimationController1 == nullptr) return;
		m_pSkinnedAnimationController1->SetTrackEnable(0, false);
		m_pSkinnedAnimationController1->SetTrackEnable(1, false);
		m_pSkinnedAnimationController1->SetTrackEnable(2, false);
		m_pSkinnedAnimationController1->SetTrackEnable(3, false);
		m_pSkinnedAnimationController1->SetTrackEnable(4, false);
		m_pSkinnedAnimationController1->SetTrackEnable(5, false);
		m_pSkinnedAnimationController1->SetTrackEnable(6, false);
		m_pSkinnedAnimationController1->SetTrackEnable(7, false);
	}
	else if (CLIENT_TYPE::OWNER == m_clientType && THIRD_PERSON_CAMERA == m_pCamera->GetMode()) // 3인칭
	{
		if (m_pSkinnedAnimationController2 == nullptr) return;
		m_pSkinnedAnimationController2->SetTrackEnable(0, false); // Idle 애니메이션 재생하도록
		m_pSkinnedAnimationController2->SetTrackEnable(1, false);
		m_pSkinnedAnimationController2->SetTrackEnable(2, false);
		m_pSkinnedAnimationController2->SetTrackEnable(3, false);

		if (m_pSkinnedAnimationController1 == nullptr) return;
		m_pSkinnedAnimationController1->SetTrackEnable(0, false);
		m_pSkinnedAnimationController1->SetTrackEnable(1, false);
		m_pSkinnedAnimationController1->SetTrackEnable(2, false);
		m_pSkinnedAnimationController1->SetTrackEnable(3, false);
		m_pSkinnedAnimationController1->SetTrackEnable(4, false);
		m_pSkinnedAnimationController1->SetTrackEnable(5, true);
		m_pSkinnedAnimationController1->SetTrackEnable(6, false);
		m_pSkinnedAnimationController1->SetTrackEnable(7, false);
	}

	
	if (CLIENT_TYPE::OTHER_PLAYER == m_clientType)
	{
		if (m_pSkinnedAnimationController2 == nullptr) return;
		m_pSkinnedAnimationController2->SetTrackEnable(0, false);
		m_pSkinnedAnimationController2->SetTrackEnable(1, false);
		m_pSkinnedAnimationController2->SetTrackEnable(2, false);
		m_pSkinnedAnimationController2->SetTrackEnable(3, false);

		if (m_pSkinnedAnimationController1 == nullptr) return;
		m_pSkinnedAnimationController1->SetTrackEnable(0, false);
		m_pSkinnedAnimationController1->SetTrackEnable(1, false);
		m_pSkinnedAnimationController1->SetTrackEnable(2, false);
		m_pSkinnedAnimationController1->SetTrackEnable(3, false);
		m_pSkinnedAnimationController1->SetTrackEnable(4, false);
		m_pSkinnedAnimationController1->SetTrackEnable(5, true);
		m_pSkinnedAnimationController1->SetTrackEnable(6, false);
		m_pSkinnedAnimationController1->SetTrackEnable(7, false);
	}
	m_pSkinnedAnimationController2->SetTrackPosition(0, 0);
	m_pSkinnedAnimationController2->SetTrackPosition(1, 0);
	m_pSkinnedAnimationController2->SetTrackPosition(2, 0);
	m_pSkinnedAnimationController2->SetTrackPosition(3, 0);

	m_pSkinnedAnimationController1->SetTrackPosition(0, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(1, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(2, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(3, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(4, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(5, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(6, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(7, 0);
}

void CEmployee::SetExitMotionAnimTrack()
{
	if (CLIENT_TYPE::OWNER == m_clientType && FIRST_PERSON_CAMERA == m_pCamera->GetMode())
	{
		// 일단 아이들 상태 애니메이션 재생하도록 한다.
		if (m_pSkinnedAnimationController2 == nullptr) return;
		m_pSkinnedAnimationController2->SetTrackEnable(0, true);
		m_pSkinnedAnimationController2->SetTrackEnable(1, false);
		m_pSkinnedAnimationController2->SetTrackEnable(2, false);
		m_pSkinnedAnimationController2->SetTrackEnable(3, false);

		if (m_pSkinnedAnimationController1 == nullptr) return;
		m_pSkinnedAnimationController1->SetTrackEnable(0, false);
		m_pSkinnedAnimationController1->SetTrackEnable(1, false);
		m_pSkinnedAnimationController1->SetTrackEnable(2, false);
		m_pSkinnedAnimationController1->SetTrackEnable(3, false);
		m_pSkinnedAnimationController1->SetTrackEnable(4, false);
		m_pSkinnedAnimationController1->SetTrackEnable(5, false);
		m_pSkinnedAnimationController1->SetTrackEnable(6, false);
	}
	else if (CLIENT_TYPE::OWNER == m_clientType && THIRD_PERSON_CAMERA == m_pCamera->GetMode())
	{
		// 일단 아이들 상태 애니메이션 재생하도록 한다.
		if (m_pSkinnedAnimationController2 == nullptr) return;
		m_pSkinnedAnimationController2->SetTrackEnable(0, false);
		m_pSkinnedAnimationController2->SetTrackEnable(1, false);
		m_pSkinnedAnimationController2->SetTrackEnable(2, false);
		m_pSkinnedAnimationController2->SetTrackEnable(3, false);

		if (m_pSkinnedAnimationController1 == nullptr) return;
		m_pSkinnedAnimationController1->SetTrackEnable(0, false);
		m_pSkinnedAnimationController1->SetTrackEnable(1, false);
		m_pSkinnedAnimationController1->SetTrackEnable(2, false);
		m_pSkinnedAnimationController1->SetTrackEnable(3, false);
		m_pSkinnedAnimationController1->SetTrackEnable(4, false);
		m_pSkinnedAnimationController1->SetTrackEnable(5, false);
		m_pSkinnedAnimationController1->SetTrackEnable(6, false);
		m_pSkinnedAnimationController1->SetTrackEnable(7, true);
	}

	// 만약 그냥 다른 플레이어라면 ~
	if (CLIENT_TYPE::OTHER_PLAYER == m_clientType)
	{
		if (m_pSkinnedAnimationController2 == nullptr) return;
		m_pSkinnedAnimationController2->SetTrackEnable(0, false);
		m_pSkinnedAnimationController2->SetTrackEnable(1, false);
		m_pSkinnedAnimationController2->SetTrackEnable(2, false);
		m_pSkinnedAnimationController2->SetTrackEnable(3, false);

		if (m_pSkinnedAnimationController1 == nullptr) return;
		m_pSkinnedAnimationController1->SetTrackEnable(0, false);
		m_pSkinnedAnimationController1->SetTrackEnable(1, false);
		m_pSkinnedAnimationController1->SetTrackEnable(2, false);
		m_pSkinnedAnimationController1->SetTrackEnable(3, false);
		m_pSkinnedAnimationController1->SetTrackEnable(4, false);
		m_pSkinnedAnimationController1->SetTrackEnable(5, false);
		m_pSkinnedAnimationController1->SetTrackEnable(6, false);
		m_pSkinnedAnimationController1->SetTrackEnable(7, true);
	}

	m_pSkinnedAnimationController2->SetTrackPosition(0, 0);
	m_pSkinnedAnimationController2->SetTrackPosition(1, 0);
	m_pSkinnedAnimationController2->SetTrackPosition(2, 0);
	m_pSkinnedAnimationController2->SetTrackPosition(3, 0);

	m_pSkinnedAnimationController1->SetTrackPosition(0, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(1, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(2, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(3, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(4, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(5, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(6, 0);
	m_pSkinnedAnimationController1->SetTrackPosition(7, 0);
}

void CEmployee::AnimTrackUpdate()
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
	case (int32)PLAYER_BEHAVIOR::RUN:
		SetRunAnimTrack();
		if (!GetOnMoveSound())
		{
			SoundManager::GetInstance().PlayObjectSound(12, 4);
			SetOnMoveSound(true);
		}
		break;
	case (int32)PLAYER_BEHAVIOR::RESCUE:
	case (int32)PLAYER_BEHAVIOR::SWITCH_INTER:
		SetInteractionAnimTrack();
		break;
	case (int32)PLAYER_BEHAVIOR::ATTACKED:
		if (m_attackedAnimationCount == EMPLOYEE_ATTACKED_TIME)
		{
			SetAttackedAnimTrack();
			m_attackedAnimationCount--;
			SoundManager::GetInstance().PlayObjectSound(13, 3);
		}
		else 
		{
			m_attackedAnimationCount--;
			m_behavior = (int32)PLAYER_BEHAVIOR::ATTACKED;
			if (m_attackedAnimationCount <= 0)
			{
				SetBehavior(PLAYER_BEHAVIOR::IDLE);
				m_bIsInvincibility = false;
			}
		}
		break;
	case (int32)PLAYER_BEHAVIOR::DOWN:
		if (m_downAnimationCount == EMPLOYEE_DOWN_TIME)
		{
			SetDownAnimTrack();
			m_downAnimationCount--;
		}
		else if (m_downAnimationCount < EMPLOYEE_DOWN_TIME)
		{
			m_downAnimationCount--;
			if (m_downAnimationCount <= 0)
			{
				SoundManager::GetInstance().PlayObjectSound(7, 3);
				SetBehavior(PLAYER_BEHAVIOR::CRAWL);
			}
		}
		break;
	case (int32)PLAYER_BEHAVIOR::CRAWL:
		SetCrawlAnimTrack();
		if (m_pCamera)
		{
			if (m_pCamera->m_fogOn)m_pCamera->m_fogOn = false;
		}
		break;

	case (int32)PLAYER_BEHAVIOR::EXIT:
		SetExitMotionAnimTrack();
		break;
	case (int32)PLAYER_BEHAVIOR::STAND:
		if (m_standAnimationCount == EMPLOYEE_STAND_TIME)
		{
			SetStandAnimTrack();
			m_standAnimationCount--;
		}
		else
		{
			if(m_standAnimationCount > 0) m_standAnimationCount--;
			if (m_standAnimationCount <= 0)
			{
				if (CLIENT_TYPE::OWNER == m_clientType)
				{
					ChangeCamera(FIRST_PERSON_CAMERA, 0);
					
					mainGame.m_SceneManager->GetSceneByIdx(3)->m_pCamera = m_pCamera;
					mainGame.m_SceneManager->GetSceneByIdx(3)->m_pCamera->CreateShaderVariables(mainGame.m_pd3dDevice, mainGame.m_pd3dCommandList);
					mainGame.m_SceneManager->GetSceneByIdx(3)->m_pCamera->m_playerIdx = m_idx;
					if (m_pCamera)
					{
						if (!m_pCamera->m_fogOn)m_pCamera->m_fogOn = true;
					}
				}
				SetBehavior(PLAYER_BEHAVIOR::IDLE);
				m_bIsInvincibility = false;
			}
		}
		break;
	}
}

CGenerator* CEmployee::GetAvailGen()
{
	CGameScene* gs = static_cast<CGameScene*>(mainGame.m_SceneManager->GetSceneByIdx((int32)CGameFramework::SCENESTATE::INGAME));
	for (int i = 0; i < 3; ++i)
	{
		XMFLOAT3 distanceVec = Vector3::Subtract(m_xmf3Position, m_pSwitches[i].position);
		float distance = Vector3::Length(distanceVec);
		float sumRange = m_playerBV.Radius + m_pSwitches[i].radius;
		if (distance <= sumRange)
		{
			CGenerator* targetGenerator = gs->GetSceneGenByIdx(i);
				
			if (targetGenerator)
			{
				if (targetGenerator->IsAvailable())
				{
					m_bIsInGenArea = true;
					m_curInterGen = i;
					return targetGenerator;
				}
				else
				{
					m_bIsInGenArea = false;
					return nullptr;
				}
			}
			else m_bIsInGenArea = false;
		}
	}
	m_bIsInGenArea = false;
	return nullptr;
}

CEmployee* CEmployee::GetAvailEMP()
{
	CGameScene* gs = static_cast<CGameScene*>(mainGame.m_SceneManager->GetSceneByIdx((int32)CGameFramework::SCENESTATE::INGAME));
	for (int i = 1; i < PLAYERNUM; ++i)
	{

		CEmployee* p = static_cast<CEmployee*>(gs->GetScenePlayerByIdx(i));
		if (p == nullptr || i == m_idx) continue;
		XMFLOAT3 ppos = p->GetPosition();
		ppos = Vector3::Subtract(m_xmf3Position, ppos);
		float dist = Vector3::Length(ppos);
		if (dist < 1.5 && p->GetBehavior() == (int32)PLAYER_BEHAVIOR::CRAWL && !p->GetRescueOn())
		{
			mainGame.m_UIRenderer->m_RescueIcon.m_hide = false;
			return p;
		}
		
	}

	mainGame.m_UIRenderer->m_RescueIcon.m_hide = true;
	return nullptr;
}

// ============== 플레이어 상태 변경 처리 ============ 05-23
void CEmployee::PlayerAttacked()
{
	if (m_hp > 0)
	{	
		m_hp -= 1;
		m_bIsInvincibility = true;


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
	if (CLIENT_TYPE::OWNER == m_clientType)
	{
		ChangeCamera(THIRD_PERSON_CAMERA, 0);
		m_pCamera->ReleaseShaderVariables();
		m_pCamera->CreateShaderVariables(mainGame.m_pd3dDevice, mainGame.m_pd3dCommandList);

		mainGame.m_SceneManager->GetSceneByIdx(3)->m_pCamera = m_pCamera;
		//mainGame.m_SceneManager->GetSceneByIdx(3)->m_pCamera->CreateShaderVariables(mainGame.m_pd3dDevice, mainGame.m_pd3dCommandList);
		m_deadCnt += 1;
	}

	SetBehavior(PLAYER_BEHAVIOR::DOWN);
	m_downAnimationCount = EMPLOYEE_DOWN_TIME;
}

bool CEmployee::GenTasking()
{
	CGenerator* targetGen = GetAvailGen();
	

	if(targetGen)std::cout << targetGen->m_idx << "Available\n";
	
	//  F키를 눌렀고, 구하기 상호작용 중이 아닐 때
	if (InputManager::GetInstance().GetKeyBuffer(KEY_TYPE::F) > 0 && !GetIsPlayerOnRescueInter())
	{
		SetBehavior(PLAYER_BEHAVIOR::IDLE);
		if (targetGen)
		{
			SetGenInteraction(true); // 캐릭터 상호작용 애니메이션 재생을 활성화 한다.
			SetBehavior(PLAYER_BEHAVIOR::SWITCH_INTER);

			targetGen->SetInteractionOn(true); // 발전기 애니메이션 재생을 시작한다.

			if (InputManager::GetInstance().GetKeyBuffer(KEY_TYPE::F) == (int8)KEY_STATUS::KEY_PRESS)
			{
				SC_EVENTPACKET packet;
				packet.eventId = m_curInterGen + (int32)EVENT_TYPE::SWITCH_ONE_START_EVENT;
				packet.size = sizeof(SC_EVENTPACKET);
				packet.type = (uint8)SC_GAME_PACKET_TYPE::GAMEEVENT;
				clientCore.DoSend(&packet);
			}
			return true;
		}
	}
	else if (!InputManager::GetInstance().GetKeyBuffer(KEY_TYPE::F))
	{
			if (InputManager::GetInstance().GetKeyBuffer(KEY_TYPE::F) == (int8)KEY_STATUS::KEY_UP)
			{
				if (GetIsPlayerOnGenInter()) // 내가 상호작용 도중이였다면
				{
					std::cout << "Cancel\n";
					SetGenInteraction(false);
					SetBehavior(PLAYER_BEHAVIOR::IDLE);
					if (targetGen) targetGen->SetInteractionOn(false);					
					//========= 패킷 송신 처리 ==============
					SC_EVENTPACKET packet;
					packet.eventId = m_curInterGen + (int32)EVENT_TYPE::SWITCH_ONE_END_EVENT;
					packet.size = sizeof(SC_EVENTPACKET);
					packet.type = (uint8)SC_GAME_PACKET_TYPE::GAMEEVENT;
					clientCore.DoSend(&packet);
				}
			}
			SetGenInteraction(false);
			SetBehavior(PLAYER_BEHAVIOR::IDLE);
	}
	return false;
}

bool CEmployee::RescueTasking()
{

	// 구조 중인 플레이어가 아닌 쓰러진 플레이어의 인덱스를 가져온다.
	CEmployee* targetPlayer = GetAvailEMP();
	// 1. 현재 켜져 있지 않고, 다른 플레이어에 의해 상호작용 중이지 않은 발전기를 가져온다.

	
		// 구하는 이벤트에 관한 패킷을 전송하도록 한다.
	if (InputManager::GetKeyBuffer(KEY_TYPE::E) == (int8)KEY_STATUS::KEY_PRESS && !GetIsPlayerOnRescueInter())
	{
		if (targetPlayer)
		{
			SetBehavior(PLAYER_BEHAVIOR::RESCUE);
			SetRescueInteraction(true);
			targetPlayer->m_bIsRescuing = true;
			SC_EVENTPACKET packet;
			packet.eventId = targetPlayer->m_idx + (int32)EVENT_TYPE::RESCUE_PLAYER_ONE;
			packet.size = sizeof(SC_EVENTPACKET);
			packet.type = (uint8)SC_GAME_PACKET_TYPE::GAMEEVENT;
			clientCore.DoSend(&packet);
		}
		return true;
	}
	else if (!InputManager::GetInstance().GetKeyBuffer(KEY_TYPE::E))
	{
		if (InputManager::GetKeyBuffer(KEY_TYPE::E) == (int8)KEY_STATUS::KEY_UP)
		{
			if (GetIsPlayerOnRescueInter())
			{
				SetBehavior(PLAYER_BEHAVIOR::IDLE);
				SetRescueInteraction(false);
				
				if (targetPlayer)
				{
					if (targetPlayer->m_bIsRescuing) targetPlayer->m_bIsRescuing = false;
					SC_EVENTPACKET packet;
					packet.eventId = targetPlayer->m_idx + (int32)EVENT_TYPE::RESCUE_CANCEL_PLAYER_ONE;
					packet.size = sizeof(SC_EVENTPACKET);
					packet.type = (uint8)SC_GAME_PACKET_TYPE::GAMEEVENT;
					clientCore.DoSend(&packet);
				}
			}
			SetRescueInteraction(false);
			SetBehavior(PLAYER_BEHAVIOR::IDLE);
		}
	}
	
	return false;
}

