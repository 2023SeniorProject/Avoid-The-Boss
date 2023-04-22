#include "pch.h"
#include "Shader.h"
#include "Player.h"
//#include "CollisionDetector.h"

CPlayer::CPlayer()
{
	m_type = 0;
	m_pCamera = NULL;
	m_xmf3Position = XMFLOAT3(0.0f, 1.25f, 0.0f);

	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Gravity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	
	m_fFriction = 0.0f;

	m_fPitch = 0.0f;
	m_fRoll = 0.0f;
	m_fYaw = 0.0f;

	m_pPlayerUpdatedContext = NULL;
	m_pCameraUpdatedContext = NULL;
}

CPlayer::~CPlayer()
{
	ReleaseShaderVariables();

	if (m_pCamera) delete m_pCamera;
}





/*플레이어의 위치를 변경하는 함수이다. 플레이어의 위치는 기본적으로 사용자가 플레이어를 이동하기 위한 키보드를
누를 때 변경된다. 플레이어의 이동 방향(dwDirection)에 따라 플레이어를 fDistance 만큼 이동한다.*/

void CPlayer::Move(DWORD dwDirection, float fDistance)
{
	XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
	if (dwDirection)
	{	
		//화살표 키 ‘↑’를 누르면 로컬 z-축 방향으로 이동(전진)한다. ‘↓’를 누르면 반대 방향으로 이동한다. 
		
		if (dwDirection & DIR_FORWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, fDistance);
		if (dwDirection & DIR_BACKWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, -fDistance);
		if (dwDirection & DIR_RIGHT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, fDistance);
		if (dwDirection & DIR_LEFT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, -fDistance);
		if (dwDirection & DIR_BUTTON_CENTER) return;
		//m_predictPos = Vector3::Add(m_xmf3Position, xmf3Shift, (1.f / 30.f)); // 30 ms 뒤의 위치를 계산
		//플레이어를 현재 위치 벡터에서 xmf3Shift 벡터만큼 이동한다
		SetVelocity(xmf3Shift);
	}
	else SetVelocity(xmf3Shift);
}

void CPlayer::SetSpeed(const XMFLOAT3& xmf3Shift)
{
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);	
}

void CPlayer::UpdateMove(const XMFLOAT3& xmf3Shift)
{
	//플레이어를 현재 위치 벡터에서 xmf3Shift 벡터만큼 이동한다. 
	m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Shift);
	//플레이어의 위치가 변경되었으므로 카메라의 위치도 xmf3Shift 벡터만큼 이동한다. 
	if (m_pCamera) m_pCamera->Move(xmf3Shift);
}

void CPlayer::Update(float fTimeElapsed, PLAYER_TYPE ptype)
{

	//플레이어를 속도 벡터 만큼 실제로 이동한다(카메라를 이동할 것이다).
	//XMFLOAT3 xmf3Velocity = Vector3::ScalarProduct(m_xmf3Velocity, fTimeElapsed, false); // 1/60 초 변경 속도를
	//UpdateMove(xmf3Velocity);
	m_playerBV.Center = GetPosition();
	/*플레이어의 위치가 변경될 때 추가로 수행할 작업을 수행한다. 플레이어의 새로운 위치가 유효한 위치가 아닐 수도
	있고 또는 플레이어의 충돌 검사 등을 수행할 필요가 있다. 이러한 상황에서 플레이어의 위치를 유효한 위치로 다시
	변경할 수 있다.*/
	


	DWORD nCameraMode = m_pCamera->GetMode();

	//플레이어의 위치가 변경되었으므로 3인칭 카메라를 갱신한다. 
	if (nCameraMode == THIRD_PERSON_CAMERA) m_pCamera->Update(m_xmf3Position,fTimeElapsed);

	//카메라의 위치가 변경될 때 추가로 수행할 작업을 수행한다. 
	if (m_pCameraUpdatedContext) OnCameraUpdateCallback();

	//카메라가 3인칭 카메라이면 카메라가 변경된 플레이어 위치를 바라보도록 한다. 
	if (nCameraMode == THIRD_PERSON_CAMERA) m_pCamera->SetLookAt(m_xmf3Position);

	//카메라의 카메라 변환 행렬을 다시 생성한다. 
	m_pCamera->RegenerateViewMatrix();
	//m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

//플레이어를 로컬 x-축, y-축, z-축을 중심으로 회전한다.
void CPlayer::Rotate(float x, float y, float z)
{
	DWORD nCameraMode = m_pCamera->GetMode();

	//1인칭 카메라 또는 3인칭 카메라의 경우 플레이어의 회전은 약간의 제약이 따른다. 
	if ((nCameraMode == FIRST_PERSON_CAMERA) || (nCameraMode == THIRD_PERSON_CAMERA))
	{

		if (x != 0.0f)
		{
			m_fPitch += x;
			if (m_fPitch > +89.0f) { x -= (m_fPitch - 89.0f); m_fPitch = +89.0f; }
			if (m_fPitch < -89.0f) { x -= (m_fPitch + 89.0f); m_fPitch = -89.0f; }
		}
		if (y != 0.0f)
		{
			
			m_fYaw += y;
			if (m_fYaw > 360.0f) m_fYaw -= 360.0f;
			if (m_fYaw < 0.0f) m_fYaw += 360.0f;
		}
		if (z != 0.0f)
		{
			
			if (m_fRoll > +20.0f) { z -= (m_fRoll - 20.0f); m_fRoll = +20.0f; }
			if (m_fRoll < -20.0f) { z -= (m_fRoll + 20.0f); m_fRoll = -20.0f; }
		}

		//카메라를 x, y, z 만큼 회전한다. 플레이어를 회전하면 카메라가 회전하게 된다. 
		m_pCamera->Rotate(x, y, z);

		/*플레이어를 회전한다. 1인칭 카메라 또는 3인칭 카메라에서 플레이어의 회전은 로컬 y-축에서만 일어난다. 플레이어
		의 로컬 y-축(Up 벡터)을 기준으로 로컬 z-축(Look 벡터)와 로컬 x-축(Right 벡터)을 회전시킨다. 기본적으로 Up 벡
		터를 기준으로 회전하는 것은 플레이어가 똑바로 서있는 것을 가정한다는 의미이다.*/
		if (y != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up),
				XMConvertToRadians(y));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
	}
	
	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);
}

void CPlayer::OnPlayerUpdateCallback()
{
	
}

void CPlayer::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pCamera) m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);

}

void CPlayer::ReleaseShaderVariables()
{
	if (m_pCamera) m_pCamera->ReleaseShaderVariables();
}

void CPlayer::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
}

CCamera* CPlayer::OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode)
{
	//새로운 카메라의 모드에 따라 카메라를 새로 생성한다. 
	CCamera* pNewCamera = NULL;
	switch (nNewCameraMode)
	{
	case FIRST_PERSON_CAMERA:
		pNewCamera = new CFirstPersonCamera(m_pCamera);
		break;
	case THIRD_PERSON_CAMERA:
		pNewCamera = new CThirdPersonCamera(m_pCamera);
		break;
	}

	if (pNewCamera)
	{
		pNewCamera->SetMode(nNewCameraMode);
		//현재 카메라를 사용하는 플레이어 객체를 설정한다. 
		pNewCamera->SetPlayer(this);
	}
	if (m_pCamera) delete m_pCamera;

	return(pNewCamera);
}

void CPlayer::OnPrepareRender()
{
	m_xmf4x4ToParent._11 = m_xmf3Right.x; m_xmf4x4ToParent._12 = m_xmf3Right.y; m_xmf4x4ToParent._13 = m_xmf3Right.z;
	m_xmf4x4ToParent._21 = m_xmf3Up.x; m_xmf4x4ToParent._22 = m_xmf3Up.y; m_xmf4x4ToParent._23 = m_xmf3Up.z;
	m_xmf4x4ToParent._31 = m_xmf3Look.x; m_xmf4x4ToParent._32 = m_xmf3Look.y; m_xmf4x4ToParent._33 = m_xmf3Look.z;
	m_xmf4x4ToParent._41 = m_xmf3Position.x; m_xmf4x4ToParent._42 = m_xmf3Position.y; m_xmf4x4ToParent._43 = m_xmf3Position.z;

	m_xmf4x4ToParent = Matrix4x4::Multiply(XMMatrixScaling(m_xmf3Scale.x, m_xmf3Scale.y, m_xmf3Scale.z), m_xmf4x4ToParent);
}

void CPlayer::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	DWORD nCameraMode = (pCamera) ? pCamera->GetMode() : 0x03;

	//카메라 모드가 3인칭이면 플레이어 객체를 렌더링한다. 
	if (nCameraMode == THIRD_PERSON_CAMERA)
	{
		CGameObject::Render(pd3dCommandList, pCamera);
	}
}

CWorker::CWorker(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_type = 0;
	m_pCamera = ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);

	m_nCharacterType = CHARACTER_TYPE::BOSS;
	m_InteractionCountTime = INTERACTION_TIME;

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

CWorker::~CWorker()
{
}

CCamera* CWorker::ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed)
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

void CWorker::OnPlayerUpdateCallback()
{
	
}

void CWorker::OnCameraUpdateCallback()
{
}

void CWorker::Move(DWORD dwDirection, float fDistance)
{
	if (dwDirection && !m_OnInteraction)
	{
		m_pSkinnedAnimationController->SetTrackEnable(0, false);
		m_pSkinnedAnimationController->SetTrackEnable(1, true);
	}
	CPlayer::Move(dwDirection, fDistance);
}

void CWorker::Update(float fTimeElapsed, PLAYER_TYPE ptype)
{
	CPlayer::Update(fTimeElapsed, PLAYER_TYPE::OWNER);

	if (m_OnInteraction)
	{
		OnInteractive();
		m_pBullet->SetOnShoot(true);
	}

	if (m_pBullet)
		m_pBullet->Update(fTimeElapsed);

	if (m_pSkinnedAnimationController&&!m_OnInteraction)
	{
		float fLength = sqrtf(m_xmf3Velocity.x * m_xmf3Velocity.x + m_xmf3Velocity.z * m_xmf3Velocity.z);
		if (::IsZero(fLength))
		{
			m_pSkinnedAnimationController->SetTrackEnable(0, true);
			m_pSkinnedAnimationController->SetTrackEnable(1, false);
			m_pSkinnedAnimationController->SetTrackPosition(0, 0.0f);
		}
	}
	if(ptype == PLAYER_TYPE::OWNER) m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);

void CWorker::OnInteractive()
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
		m_InteractionCountTime = INTERACTION_TIME;
	}
}

CEmployee::CEmployee(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CHARACTER_TYPE nType)
{
	m_pCamera = ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);

	m_nCharacterType = nType;

	m_InteractionCountTime = INTERACTION_TIME;

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

	SetScale(XMFLOAT3(1.f, 1.f, 1.f));
	SetPosition(XMFLOAT3(0.0f, 0.25f, -50.0f));

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

void CEmployee::OnPlayerUpdateCallback()
{
}

void CEmployee::OnCameraUpdateCallback()
{
}

void CEmployee::Move(DWORD dwDirection, float fDistance)
{
	if (dwDirection && !m_OnInteraction)
	{
		m_pSkinnedAnimationController->SetTrackEnable(0, false);
		m_pSkinnedAnimationController->SetTrackEnable(1, true);
	}

	CPlayer::Move(dwDirection, fDistance);
}

void CEmployee::Update(float fTimeElapsed, PLAYER_TYPE ptype)
{
	CPlayer::Update(fTimeElapsed, ptype);

	if (CheckSwitchArea())
	{
		SetSwitchArea(true);
	}
	else
		SetSwitchArea(false);

	if (m_OnInteraction)
			OnInteractive();
	
	if (m_pSkinnedAnimationController&&!m_OnInteraction)
	{
		float fLength = sqrtf(m_xmf3Velocity.x * m_xmf3Velocity.x + m_xmf3Velocity.z * m_xmf3Velocity.z);
		if (::IsZero(fLength))
		{
			m_pSkinnedAnimationController->SetTrackEnable(0, true);
			m_pSkinnedAnimationController->SetTrackEnable(1, false);
			m_pSkinnedAnimationController->SetTrackPosition(0, 0.0f);
		}
	}
	if(ptype == PLAYER_TYPE::OWNER) m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
}
void CEmployee::OnInteractive()
{
	if (m_OnInteraction == true && m_InteractionCountTime > 0)
	{
		m_pSkinnedAnimationController->SetTrackEnable(0, false);
		m_pSkinnedAnimationController->SetTrackEnable(1, false);
		m_pSkinnedAnimationController->SetTrackEnable(6, true);
		m_pSkinnedAnimationController->SetTrackPosition(0, 0.0f);

		m_InteractionCountTime -= 1;
	}
	else
	{
		m_pSkinnedAnimationController->SetTrackEnable(0, true);
		m_pSkinnedAnimationController->SetTrackEnable(1, false);
		m_pSkinnedAnimationController->SetTrackEnable(6, false);
		m_OnInteraction = false;
		m_InteractionCountTime = INTERACTION_TIME;
	}
}
bool CEmployee::CheckSwitchArea()
{
	//캐릭터 원 - 스위치 영역 원 충돌체크
	XMFLOAT3 v1 = m_pSwitch.position;
	XMFLOAT3 v2 = m_xmf3Position;

	float fDistance = sqrt(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2) + pow(v1.y -v2.y, 2));
	float fSumRange = m_pSwitch.radius + m_playerBV.Radius + 1.0f;

	if (fDistance <= fSumRange)
		return true;
	else
		return false;

	return false;
}

#define _WITH_DEBUG_CALLBACK_DATA

void CSoundCallbackHandler::HandleCallback(void* pCallbackData, float fTrackPosition)
{
	_TCHAR* pWavName = (_TCHAR*)pCallbackData;
#ifdef _WITH_DEBUG_CALLBACK_DATA
	TCHAR pstrDebug[256] = { 0 };
	_stprintf_s(pstrDebug, 256, _T("%s(%f)\n"), pWavName, fTrackPosition);
	OutputDebugString(pstrDebug);
#endif
#ifdef _WITH_SOUND_RESOURCE
	PlaySound(pWavName, ::ghAppInstance, SND_RESOURCE | SND_ASYNC);
#else
	PlaySound(pWavName, NULL, SND_FILENAME | SND_ASYNC);
#endif
}


CBullet::CBullet(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CLoadedModelInfo* pBulletModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Sphere.bin", NULL, Layout::Bullet);
	SetChild(pBulletModel->m_pModelRootObject, true);
}

CBullet::~CBullet()
{
}

void CBullet::Update(float fTimeElapsed)
{
	if (m_OnShoot)
		CGameObject::MoveForward(1.0f);
}
